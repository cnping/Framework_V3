/*
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/


//#define LOG_NDEBUG 0
#define LOG_TAG "AudioTrack"

#include <stdint.h>
#include <sys/types.h>
#include <limits.h>

#include <sched.h>
#include <sys/resource.h>

#include <private/media/AudioTrackShared.h>

#include <media/AudioSystem.h>
#include <media/AudioTrack.h>

#include <utils/Log.h>
#include <binder/Parcel.h>
#include <binder/IPCThreadState.h>
#include <utils/Timers.h>
#include <utils/Atomic.h>

#include <cutils/bitops.h>
#include <cutils/compiler.h>

#include <system/audio.h>
#include <system/audio_policy.h>

#include <audio_utils/primitives.h>

namespace android {
// ---------------------------------------------------------------------------

// static
status_t AudioTrack::getMinFrameCount(
        int* frameCount,
        audio_stream_type_t streamType,
        uint32_t sampleRate)
{
    if (frameCount == NULL) return BAD_VALUE;

    // default to 0 in case of error
    *frameCount = 0;

    // FIXME merge with similar code in createTrack_l(), except we're missing
    //       some information here that is available in createTrack_l():
    //          audio_io_handle_t output
    //          audio_format_t format
    //          audio_channel_mask_t channelMask
    //          audio_output_flags_t flags
    int afSampleRate;
    if (AudioSystem::getOutputSamplingRate(&afSampleRate, streamType) != NO_ERROR) {
        return NO_INIT;
    }
    int afFrameCount;
    if (AudioSystem::getOutputFrameCount(&afFrameCount, streamType) != NO_ERROR) {
        return NO_INIT;
    }
    uint32_t afLatency;
    if (AudioSystem::getOutputLatency(&afLatency, streamType) != NO_ERROR) {
        return NO_INIT;
    }

    // Ensure that buffer depth covers at least audio hardware latency
    uint32_t minBufCount = afLatency / ((1000 * afFrameCount) / afSampleRate);
    if (minBufCount < 2) minBufCount = 2;

    *frameCount = (sampleRate == 0) ? afFrameCount * minBufCount :
            afFrameCount * minBufCount * sampleRate / afSampleRate;
    ALOGV("getMinFrameCount=%d: afFrameCount=%d, minBufCount=%d, afSampleRate=%d, afLatency=%d",
            *frameCount, afFrameCount, minBufCount, afSampleRate, afLatency);
    return NO_ERROR;
}

// ---------------------------------------------------------------------------

AudioTrack::AudioTrack()
    : mStatus(NO_INIT),
      mIsTimed(false),
      mPreviousPriority(ANDROID_PRIORITY_NORMAL),
      mPreviousSchedulingGroup(SP_DEFAULT)
{
}

AudioTrack::AudioTrack(
        audio_stream_type_t streamType,
        uint32_t sampleRate,
        audio_format_t format,
        audio_channel_mask_t channelMask,
        int frameCount,
        audio_output_flags_t flags,
        callback_t cbf,
        void* user,
        int notificationFrames,
        int sessionId)
    : mStatus(NO_INIT),
      mIsTimed(false),
      mPreviousPriority(ANDROID_PRIORITY_NORMAL),
      mPreviousSchedulingGroup(SP_DEFAULT)
{
    mStatus = set(streamType, sampleRate, format, channelMask,
            frameCount, flags, cbf, user, notificationFrames,
            0 /*sharedBuffer*/, false /*threadCanCallJava*/, sessionId);
}

// DEPRECATED
AudioTrack::AudioTrack(
        int streamType,
        uint32_t sampleRate,
        int format,
        int channelMask,
        int frameCount,
        uint32_t flags,
        callback_t cbf,
        void* user,
        int notificationFrames,
        int sessionId)
    : mStatus(NO_INIT),
      mIsTimed(false),
      mPreviousPriority(ANDROID_PRIORITY_NORMAL), mPreviousSchedulingGroup(SP_DEFAULT)
{
    mStatus = set((audio_stream_type_t)streamType, sampleRate, (audio_format_t)format,
            (audio_channel_mask_t) channelMask,
            frameCount, (audio_output_flags_t)flags, cbf, user, notificationFrames,
            0 /*sharedBuffer*/, false /*threadCanCallJava*/, sessionId);
}

AudioTrack::AudioTrack(
        audio_stream_type_t streamType,
        uint32_t sampleRate,
        audio_format_t format,
        audio_channel_mask_t channelMask,
        const sp<IMemory>& sharedBuffer,
        audio_output_flags_t flags,
        callback_t cbf,
        void* user,
        int notificationFrames,
        int sessionId)
    : mStatus(NO_INIT),
      mIsTimed(false),
      mPreviousPriority(ANDROID_PRIORITY_NORMAL),
      mPreviousSchedulingGroup(SP_DEFAULT)
{
    mStatus = set(streamType, sampleRate, format, channelMask,
            0 /*frameCount*/, flags, cbf, user, notificationFrames,
            sharedBuffer, false /*threadCanCallJava*/, sessionId);
}

AudioTrack::~AudioTrack()
{
    ALOGV_IF(mSharedBuffer != 0, "Destructor sharedBuffer: %p", mSharedBuffer->pointer());

    if (mStatus == NO_ERROR) {
        // Make sure that callback function exits in the case where
        // it is looping on buffer full condition in obtainBuffer().
        // Otherwise the callback thread will never exit.
        stop();
        if (mAudioTrackThread != 0) {
            mAudioTrackThread->requestExit();   // see comment in AudioTrack.h
            mAudioTrackThread->requestExitAndWait();
            mAudioTrackThread.clear();
        }
        mAudioTrack.clear();
        IPCThreadState::self()->flushCommands();
        AudioSystem::releaseAudioSessionId(mSessionId);
    }
}

status_t AudioTrack::set(
        audio_stream_type_t streamType,
        uint32_t sampleRate,
        audio_format_t format,
        audio_channel_mask_t channelMask,
        int frameCount,
        audio_output_flags_t flags,
        callback_t cbf,
        void* user,
        int notificationFrames,
        const sp<IMemory>& sharedBuffer,
        bool threadCanCallJava,
        int sessionId)
{

    ALOGV_IF(sharedBuffer != 0, "sharedBuffer: %p, size: %d", sharedBuffer->pointer(), sharedBuffer->size());

    ALOGV("set() streamType %d frameCount %d flags %04x", streamType, frameCount, flags);

    AutoMutex lock(mLock);
    if (mAudioTrack != 0) {
        ALOGE("Track already in use");
        return INVALID_OPERATION;
    }

    // handle default values first.
    if (streamType == AUDIO_STREAM_DEFAULT) {
        streamType = AUDIO_STREAM_MUSIC;
    }

    if (sampleRate == 0) {
        int afSampleRate;
        if (AudioSystem::getOutputSamplingRate(&afSampleRate, streamType) != NO_ERROR) {
            return NO_INIT;
        }
        sampleRate = afSampleRate;
    }

    // these below should probably come from the audioFlinger too...
    if (format == AUDIO_FORMAT_DEFAULT) {
        format = AUDIO_FORMAT_PCM_16_BIT;
    }
    if (channelMask == 0) {
        channelMask = AUDIO_CHANNEL_OUT_STEREO;
    }

    // validate parameters
    if (!audio_is_valid_format(format)) {
        ALOGE("Invalid format");
        return BAD_VALUE;
    }

    // AudioFlinger does not currently support 8-bit data in shared memory
    if (format == AUDIO_FORMAT_PCM_8_BIT && sharedBuffer != 0) {
        ALOGE("8-bit data in shared memory is not supported");
        return BAD_VALUE;
    }

    // force direct flag if format is not linear PCM
    if (!audio_is_linear_pcm(format)) {
        flags = (audio_output_flags_t)
                // FIXME why can't we allow direct AND fast?
                ((flags | AUDIO_OUTPUT_FLAG_DIRECT) & ~AUDIO_OUTPUT_FLAG_FAST);
    }
    // only allow deep buffering for music stream type
    if (streamType != AUDIO_STREAM_MUSIC) {
        flags = (audio_output_flags_t)(flags &~AUDIO_OUTPUT_FLAG_DEEP_BUFFER);
    }

    if (!audio_is_output_channel(channelMask)) {
        ALOGE("Invalid channel mask %#x", channelMask);
        return BAD_VALUE;
    }
    uint32_t channelCount = popcount(channelMask);

    audio_io_handle_t output = AudioSystem::getOutput(
                                    streamType,
                                    sampleRate, format, channelMask,
                                    flags);

    if (output == 0) {
        ALOGE("Could not get audio output for stream type %d", streamType);
        return BAD_VALUE;
    }

    mVolume[LEFT] = 1.0f;
    mVolume[RIGHT] = 1.0f;
    mSendLevel = 0.0f;
    mFrameCount = frameCount;
    mNotificationFramesReq = notificationFrames;
    mSessionId = sessionId;
    mAuxEffectId = 0;
    mFlags = flags;
    mCbf = cbf;

    if (cbf != NULL) {
        mAudioTrackThread = new AudioTrackThread(*this, threadCanCallJava);
        mAudioTrackThread->run("AudioTrack", ANDROID_PRIORITY_AUDIO, 0 /*stack*/);
    }

    // create the IAudioTrack
    status_t status = createTrack_l(streamType,
                                  sampleRate,
                                  format,
                                  channelMask,
                                  frameCount,
                                  flags,
                                  sharedBuffer,
                                  output);

    if (status != NO_ERROR) {
        if (mAudioTrackThread != 0) {
            mAudioTrackThread->requestExit();
            mAudioTrackThread.clear();
        }
        return status;
    }

    mStatus = NO_ERROR;

    mStreamType = streamType;
    mFormat = format;
    mChannelMask = channelMask;
    mChannelCount = channelCount;
    mSharedBuffer = sharedBuffer;
    mMuted = false;
    mActive = false;
    mUserData = user;
    mLoopCount = 0;
    mMarkerPosition = 0;
    mMarkerReached = false;
    mNewPosition = 0;
    mUpdatePeriod = 0;
    mFlushed = false;
    AudioSystem::acquireAudioSessionId(mSessionId);
    mRestoreStatus = NO_ERROR;
    return NO_ERROR;
}

status_t AudioTrack::initCheck() const
{
    return mStatus;
}

// -------------------------------------------------------------------------

uint32_t AudioTrack::latency() const
{
    return mLatency;
}

audio_stream_type_t AudioTrack::streamType() const
{
    return mStreamType;
}

audio_format_t AudioTrack::format() const
{
    return mFormat;
}

int AudioTrack::channelCount() const
{
    return mChannelCount;
}

uint32_t AudioTrack::frameCount() const
{
    return mCblk->frameCount;
}

size_t AudioTrack::frameSize() const
{
    if (audio_is_linear_pcm(mFormat)) {
        return channelCount()*audio_bytes_per_sample(mFormat);
    } else {
        return sizeof(uint8_t);
    }
}

sp<IMemory>& AudioTrack::sharedBuffer()
{
    return mSharedBuffer;
}

// -------------------------------------------------------------------------

void AudioTrack::start()
{
    sp<AudioTrackThread> t = mAudioTrackThread;

    ALOGV("start %p", this);

    AutoMutex lock(mLock);
    // acquire a strong reference on the IMemory and IAudioTrack so that they cannot be destroyed
    // while we are accessing the cblk
    sp<IAudioTrack> audioTrack = mAudioTrack;
    sp<IMemory> iMem = mCblkMemory;
    audio_track_cblk_t* cblk = mCblk;

    if (!mActive) {
        mFlushed = false;
        mActive = true;
        mNewPosition = cblk->server + mUpdatePeriod;
        cblk->lock.lock();
        cblk->bufferTimeoutMs = MAX_STARTUP_TIMEOUT_MS;
        cblk->waitTimeMs = 0;
        android_atomic_and(~CBLK_DISABLED_ON, &cblk->flags);
        if (t != 0) {
            t->resume();
        } else {
            mPreviousPriority = getpriority(PRIO_PROCESS, 0);
            get_sched_policy(0, &mPreviousSchedulingGroup);
            androidSetThreadPriority(0, ANDROID_PRIORITY_AUDIO);
        }

        ALOGV("start %p before lock cblk %p", this, mCblk);
        status_t status = NO_ERROR;
        if (!(cblk->flags & CBLK_INVALID_MSK)) {
            cblk->lock.unlock();
            ALOGV("mAudioTrack->start()");
            status = mAudioTrack->start();
            cblk->lock.lock();
            if (status == DEAD_OBJECT) {
                android_atomic_or(CBLK_INVALID_ON, &cblk->flags);
            }
        }
        if (cblk->flags & CBLK_INVALID_MSK) {
            status = restoreTrack_l(cblk, true);
        }
        cblk->lock.unlock();
        if (status != NO_ERROR) {
            ALOGV("start() failed");
            mActive = false;
            if (t != 0) {
                t->pause();
            } else {
                setpriority(PRIO_PROCESS, 0, mPreviousPriority);
                set_sched_policy(0, mPreviousSchedulingGroup);
            }
        }
    }

}

void AudioTrack::stop()
{
    sp<AudioTrackThread> t = mAudioTrackThread;

    ALOGV("stop %p", this);

    AutoMutex lock(mLock);
    if (mActive) {
        mActive = false;
        mCblk->cv.signal();
        mAudioTrack->stop();
        // Cancel loops (If we are in the middle of a loop, playback
        // would not stop until loopCount reaches 0).
        setLoop_l(0, 0, 0);
        // the playback head position will reset to 0, so if a marker is set, we need
        // to activate it again
        mMarkerReached = false;
        // Force flush if a shared buffer is used otherwise audioflinger
        // will not stop before end of buffer is reached.
        if (mSharedBuffer != 0) {
            flush_l();
        }
        if (t != 0) {
            t->pause();
        } else {
            setpriority(PRIO_PROCESS, 0, mPreviousPriority);
            set_sched_policy(0, mPreviousSchedulingGroup);
        }
    }

}

bool AudioTrack::stopped() const
{
    AutoMutex lock(mLock);
    return stopped_l();
}

void AudioTrack::flush()
{
    AutoMutex lock(mLock);
    flush_l();
}

// must be called with mLock held
void AudioTrack::flush_l()
{
    ALOGV("flush");

    // clear playback marker and periodic update counter
    mMarkerPosition = 0;
    mMarkerReached = false;
    mUpdatePeriod = 0;

    if (!mActive) {
        mFlushed = true;
        mAudioTrack->flush();
        // Release AudioTrack callback thread in case it was waiting for new buffers
        // in AudioTrack::obtainBuffer()
        mCblk->cv.signal();
    }
}

void AudioTrack::pause()
{
    ALOGV("pause");
    AutoMutex lock(mLock);
    if (mActive) {
        mActive = false;
        mCblk->cv.signal();
        mAudioTrack->pause();
    }
}

void AudioTrack::mute(bool e)
{
    mAudioTrack->mute(e);
    mMuted = e;
}

bool AudioTrack::muted() const
{
    return mMuted;
}

status_t AudioTrack::setVolume(float left, float right)
{
    if (left < 0.0f || left > 1.0f || right < 0.0f || right > 1.0f) {
        return BAD_VALUE;
    }

    AutoMutex lock(mLock);
    mVolume[LEFT] = left;
    mVolume[RIGHT] = right;

    mCblk->setVolumeLR((uint32_t(uint16_t(right * 0x1000)) << 16) | uint16_t(left * 0x1000));

    return NO_ERROR;
}

void AudioTrack::getVolume(float* left, float* right) const
{
    if (left != NULL) {
        *left  = mVolume[LEFT];
    }
    if (right != NULL) {
        *right = mVolume[RIGHT];
    }
}

status_t AudioTrack::setAuxEffectSendLevel(float level)
{
    ALOGV("setAuxEffectSendLevel(%f)", level);
    if (level < 0.0f || level > 1.0f) {
        return BAD_VALUE;
    }
    AutoMutex lock(mLock);

    mSendLevel = level;

    mCblk->setSendLevel(level);

    return NO_ERROR;
}

void AudioTrack::getAuxEffectSendLevel(float* level) const
{
    if (level != NULL) {
        *level  = mSendLevel;
    }
}

status_t AudioTrack::setSampleRate(int rate)
{
    int afSamplingRate;

    if (mIsTimed) {
        return INVALID_OPERATION;
    }

    if (AudioSystem::getOutputSamplingRate(&afSamplingRate, mStreamType) != NO_ERROR) {
        return NO_INIT;
    }
    // Resampler implementation limits input sampling rate to 2 x output sampling rate.
    // limit outout sampling rate to 96k
    if (rate <= 0 || rate > 48000 * 2 ) return BAD_VALUE;

    AutoMutex lock(mLock);
    mCblk->sampleRate = rate;
    return NO_ERROR;
}

uint32_t AudioTrack::getSampleRate() const
{
    if (mIsTimed) {
        return INVALID_OPERATION;
    }

    AutoMutex lock(mLock);
    return mCblk->sampleRate;
}

status_t AudioTrack::setLoop(uint32_t loopStart, uint32_t loopEnd, int loopCount)
{
    AutoMutex lock(mLock);
    return setLoop_l(loopStart, loopEnd, loopCount);
}

// must be called with mLock held
status_t AudioTrack::setLoop_l(uint32_t loopStart, uint32_t loopEnd, int loopCount)
{
    audio_track_cblk_t* cblk = mCblk;

    Mutex::Autolock _l(cblk->lock);

    if (loopCount == 0) {
        cblk->loopStart = UINT_MAX;
        cblk->loopEnd = UINT_MAX;
        cblk->loopCount = 0;
        mLoopCount = 0;
        return NO_ERROR;
    }

    if (mIsTimed) {
        return INVALID_OPERATION;
    }

    if (loopStart >= loopEnd ||
        loopEnd - loopStart > cblk->frameCount ||
        cblk->server > loopStart) {
        ALOGE("setLoop invalid value: loopStart %d, loopEnd %d, loopCount %d, framecount %d, user %d", loopStart, loopEnd, loopCount, cblk->frameCount, cblk->user);
        return BAD_VALUE;
    }

    if ((mSharedBuffer != 0) && (loopEnd > cblk->frameCount)) {
        ALOGE("setLoop invalid value: loop markers beyond data: loopStart %d, loopEnd %d, framecount %d",
            loopStart, loopEnd, cblk->frameCount);
        return BAD_VALUE;
    }

    cblk->loopStart = loopStart;
    cblk->loopEnd = loopEnd;
    cblk->loopCount = loopCount;
    mLoopCount = loopCount;

    return NO_ERROR;
}

status_t AudioTrack::setMarkerPosition(uint32_t marker)
{
    if (mCbf == NULL) return INVALID_OPERATION;

    mMarkerPosition = marker;
    mMarkerReached = false;

    return NO_ERROR;
}

status_t AudioTrack::getMarkerPosition(uint32_t *marker) const
{
    if (marker == NULL) return BAD_VALUE;

    *marker = mMarkerPosition;

    return NO_ERROR;
}

status_t AudioTrack::setPositionUpdatePeriod(uint32_t updatePeriod)
{
    if (mCbf == NULL) return INVALID_OPERATION;

    uint32_t curPosition;
    getPosition(&curPosition);
    mNewPosition = curPosition + updatePeriod;
    mUpdatePeriod = updatePeriod;

    return NO_ERROR;
}

status_t AudioTrack::getPositionUpdatePeriod(uint32_t *updatePeriod) const
{
    if (updatePeriod == NULL) return BAD_VALUE;

    *updatePeriod = mUpdatePeriod;

    return NO_ERROR;
}

status_t AudioTrack::setPosition(uint32_t position)
{
    if (mIsTimed) return INVALID_OPERATION;

    AutoMutex lock(mLock);

    if (!stopped_l()) return INVALID_OPERATION;

    Mutex::Autolock _l(mCblk->lock);

    if (position > mCblk->user) return BAD_VALUE;

    mCblk->server = position;
    android_atomic_or(CBLK_FORCEREADY_ON, &mCblk->flags);

    return NO_ERROR;
}

status_t AudioTrack::getPosition(uint32_t *position)
{
    if (position == NULL) return BAD_VALUE;
    AutoMutex lock(mLock);
    *position = mFlushed ? 0 : mCblk->server;

    return NO_ERROR;
}

status_t AudioTrack::reload()
{
    AutoMutex lock(mLock);

    if (!stopped_l()) return INVALID_OPERATION;

    flush_l();

    mCblk->stepUser(mCblk->frameCount);

    return NO_ERROR;
}

audio_io_handle_t AudioTrack::getOutput()
{
    AutoMutex lock(mLock);
    return getOutput_l();
}

// must be called with mLock held
audio_io_handle_t AudioTrack::getOutput_l()
{
    return AudioSystem::getOutput(mStreamType,
            mCblk->sampleRate, mFormat, mChannelMask, mFlags);
}

int AudioTrack::getSessionId() const
{
    return mSessionId;
}

status_t AudioTrack::attachAuxEffect(int effectId)
{
    ALOGV("attachAuxEffect(%d)", effectId);
    status_t status = mAudioTrack->attachAuxEffect(effectId);
    if (status == NO_ERROR) {
        mAuxEffectId = effectId;
    }
    return status;
}

// -------------------------------------------------------------------------

// must be called with mLock held
status_t AudioTrack::createTrack_l(
        audio_stream_type_t streamType,
        uint32_t sampleRate,
        audio_format_t format,
        audio_channel_mask_t channelMask,
        int frameCount,
        audio_output_flags_t flags,
        const sp<IMemory>& sharedBuffer,
        audio_io_handle_t output)
{
    status_t status;
    const sp<IAudioFlinger>& audioFlinger = AudioSystem::get_audio_flinger();
    if (audioFlinger == 0) {
        ALOGE("Could not get audioflinger");
        return NO_INIT;
    }

    uint32_t afLatency;
    if (AudioSystem::getLatency(output, streamType, &afLatency) != NO_ERROR) {
        return NO_INIT;
    }

    // Client decides whether the track is TIMED (see below), but can only express a preference
    // for FAST.  Server will perform additional tests.
    if ((flags & AUDIO_OUTPUT_FLAG_FAST) && !(
            // either of these use cases:
            // use case 1: shared buffer
            (sharedBuffer != 0) ||
            // use case 2: callback handler
            (mCbf != NULL))) {
        ALOGW("AUDIO_OUTPUT_FLAG_FAST denied by client");
        // once denied, do not request again if IAudioTrack is re-created
        flags = (audio_output_flags_t) (flags & ~AUDIO_OUTPUT_FLAG_FAST);
        mFlags = flags;
    }
    ALOGV("createTrack_l() output %d afLatency %d", output, afLatency);

    mNotificationFramesAct = mNotificationFramesReq;

    if (!audio_is_linear_pcm(format)) {

        if (sharedBuffer != 0) {
            // Same comment as below about ignoring frameCount parameter for set()
            frameCount = sharedBuffer->size();
        } else if (frameCount == 0) {
            int afFrameCount;
            if (AudioSystem::getFrameCount(output, streamType, &afFrameCount) != NO_ERROR) {
                return NO_INIT;
            }
            frameCount = afFrameCount;
        }

    } else if (sharedBuffer != 0) {

        // Ensure that buffer alignment matches channelCount
        int channelCount = popcount(channelMask);
        // 8-bit data in shared memory is not currently supported by AudioFlinger
        size_t alignment = /* format == AUDIO_FORMAT_PCM_8_BIT ? 1 : */ 2;
        if (channelCount > 1) {
            // More than 2 channels does not require stronger alignment than stereo
            alignment <<= 1;
        }
        if (((uint32_t)sharedBuffer->pointer() & (alignment - 1)) != 0) {
            ALOGE("Invalid buffer alignment: address %p, channelCount %d",
                    sharedBuffer->pointer(), channelCount);
            return BAD_VALUE;
        }

        // When initializing a shared buffer AudioTrack via constructors,
        // there's no frameCount parameter.
        // But when initializing a shared buffer AudioTrack via set(),
        // there _is_ a frameCount parameter.  We silently ignore it.
        frameCount = sharedBuffer->size()/channelCount/sizeof(int16_t);

    } else if (!(flags & AUDIO_OUTPUT_FLAG_FAST)) {

        // FIXME move these calculations and associated checks to server
        int afSampleRate;
        if (AudioSystem::getSamplingRate(output, streamType, &afSampleRate) != NO_ERROR) {
            return NO_INIT;
        }
        int afFrameCount;
        if (AudioSystem::getFrameCount(output, streamType, &afFrameCount) != NO_ERROR) {
            return NO_INIT;
        }

        // Ensure that buffer depth covers at least audio hardware latency
        uint32_t minBufCount = afLatency / ((1000 * afFrameCount)/afSampleRate);
        if (minBufCount < 2) minBufCount = 2;

        int minFrameCount = (afFrameCount*sampleRate*minBufCount)/afSampleRate;
        ALOGV("minFrameCount: %d, afFrameCount=%d, minBufCount=%d, sampleRate=%d, afSampleRate=%d"
                ", afLatency=%d",
                minFrameCount, afFrameCount, minBufCount, sampleRate, afSampleRate, afLatency);

        if (frameCount == 0) {
            frameCount = minFrameCount;
        }
        if (mNotificationFramesAct == 0) {
            mNotificationFramesAct = frameCount/2;
        }
        // Make sure that application is notified with sufficient margin
        // before underrun
        if (mNotificationFramesAct > (uint32_t)frameCount/2) {
            mNotificationFramesAct = frameCount/2;
        }
        if (frameCount < minFrameCount) {
            // not ALOGW because it happens all the time when playing key clicks over A2DP
            ALOGV("Minimum buffer size corrected from %d to %d",
                     frameCount, minFrameCount);
            frameCount = minFrameCount;
        }

    } else {
        // For fast tracks, the frame count calculations and checks are done by server
    }

    IAudioFlinger::track_flags_t trackFlags = IAudioFlinger::TRACK_DEFAULT;
    if (mIsTimed) {
        trackFlags |= IAudioFlinger::TRACK_TIMED;
    }

    pid_t tid = -1;
    if (flags & AUDIO_OUTPUT_FLAG_FAST) {
        trackFlags |= IAudioFlinger::TRACK_FAST;
        if (mAudioTrackThread != 0) {
            tid = mAudioTrackThread->getTid();
        }
    }

    sp<IAudioTrack> track = audioFlinger->createTrack(getpid(),
                                                      streamType,
                                                      sampleRate,
                                                      format,
                                                      channelMask,
                                                      frameCount,
                                                      trackFlags,
                                                      sharedBuffer,
                                                      output,
                                                      tid,
                                                      &mSessionId,
                                                      &status);

    if (track == 0) {
        ALOGE("AudioFlinger could not create track, status: %d", status);
        return status;
    }
    sp<IMemory> cblk = track->getCblk();
    if (cblk == 0) {
        ALOGE("Could not get control block");
        return NO_INIT;
    }
    mAudioTrack = track;
    mCblkMemory = cblk;
    mCblk = static_cast<audio_track_cblk_t*>(cblk->pointer());
    // old has the previous value of mCblk->flags before the "or" operation
    int32_t old = android_atomic_or(CBLK_DIRECTION_OUT, &mCblk->flags);
    if (flags & AUDIO_OUTPUT_FLAG_FAST) {
        if (old & CBLK_FAST) {
            ALOGV("AUDIO_OUTPUT_FLAG_FAST successful; frameCount %u", mCblk->frameCount);
        } else {
            ALOGV("AUDIO_OUTPUT_FLAG_FAST denied by server; frameCount %u", mCblk->frameCount);
            // once denied, do not request again if IAudioTrack is re-created
            flags = (audio_output_flags_t) (flags & ~AUDIO_OUTPUT_FLAG_FAST);
            mFlags = flags;
        }
        if (sharedBuffer == 0) {
            mNotificationFramesAct = mCblk->frameCount/2;
        }
    }
    if (sharedBuffer == 0) {
        mCblk->buffers = (char*)mCblk + sizeof(audio_track_cblk_t);
    } else {
        mCblk->buffers = sharedBuffer->pointer();
        // Force buffer full condition as data is already present in shared memory
        mCblk->stepUser(mCblk->frameCount);
    }

    mCblk->setVolumeLR((uint32_t(uint16_t(mVolume[RIGHT] * 0x1000)) << 16) | uint16_t(mVolume[LEFT] * 0x1000));
    mCblk->setSendLevel(mSendLevel);
    mAudioTrack->attachAuxEffect(mAuxEffectId);
    mCblk->bufferTimeoutMs = MAX_STARTUP_TIMEOUT_MS;
    mCblk->waitTimeMs = 0;
    mRemainingFrames = mNotificationFramesAct;
    // FIXME don't believe this lie
    mLatency = afLatency + (1000*mCblk->frameCount) / sampleRate;
    // If IAudioTrack is re-created, don't let the requested frameCount
    // decrease.  This can confuse clients that cache frameCount().
    if (mCblk->frameCount > mFrameCount) {
        mFrameCount = mCblk->frameCount;
    }
    return NO_ERROR;
}

status_t AudioTrack::obtainBuffer(Buffer* audioBuffer, int32_t waitCount)
{
    AutoMutex lock(mLock);
    bool active;
    status_t result = NO_ERROR;
    audio_track_cblk_t* cblk = mCblk;
    uint32_t framesReq = audioBuffer->frameCount;
    uint32_t waitTimeMs = (waitCount < 0) ? cblk->bufferTimeoutMs : WAIT_PERIOD_MS;

    audioBuffer->frameCount  = 0;
    audioBuffer->size = 0;

    uint32_t framesAvail = cblk->framesAvailable();

    cblk->lock.lock();
    if (cblk->flags & CBLK_INVALID_MSK) {
        goto create_new_track;
    }
    cblk->lock.unlock();

    if (framesAvail == 0) {
        cblk->lock.lock();
        goto start_loop_here;
        while (framesAvail == 0) {
            active = mActive;
            if (CC_UNLIKELY(!active)) {
                ALOGV("Not active and NO_MORE_BUFFERS");
                cblk->lock.unlock();
                return NO_MORE_BUFFERS;
            }
            if (CC_UNLIKELY(!waitCount)) {
                cblk->lock.unlock();
                return WOULD_BLOCK;
            }
            if (!(cblk->flags & CBLK_INVALID_MSK)) {
                mLock.unlock();
                result = cblk->cv.waitRelative(cblk->lock, milliseconds(waitTimeMs));
                cblk->lock.unlock();
                mLock.lock();
                if (!mActive) {
                    return status_t(STOPPED);
                }
                cblk->lock.lock();
            }

            if (cblk->flags & CBLK_INVALID_MSK) {
                goto create_new_track;
            }
            if (CC_UNLIKELY(result != NO_ERROR)) {
                cblk->waitTimeMs += waitTimeMs;
                if (cblk->waitTimeMs >= cblk->bufferTimeoutMs) {
                    // timing out when a loop has been set and we have already written upto loop end
                    // is a normal condition: no need to wake AudioFlinger up.
                    if (cblk->user < cblk->loopEnd) {
                        ALOGW(   "obtainBuffer timed out (is the CPU pegged?) %p name=%#x"
                                "user=%08x, server=%08x", this, cblk->mName, cblk->user, cblk->server);
                        //unlock cblk mutex before calling mAudioTrack->start() (see issue #1617140)
                        cblk->lock.unlock();
                        result = mAudioTrack->start();
                        cblk->lock.lock();
                        if (result == DEAD_OBJECT) {
                            android_atomic_or(CBLK_INVALID_ON, &cblk->flags);
create_new_track:
                            result = restoreTrack_l(cblk, false);
                        }
                        if (result != NO_ERROR) {
                            ALOGW("obtainBuffer create Track error %d", result);
                            cblk->lock.unlock();
                            return result;
                        }
                    }
                    cblk->waitTimeMs = 0;
                }

                if (--waitCount == 0) {
                    cblk->lock.unlock();
                    return TIMED_OUT;
                }
            }
            // read the server count again
        start_loop_here:
            framesAvail = cblk->framesAvailable_l();
        }
        cblk->lock.unlock();
    }

    cblk->waitTimeMs = 0;

    if (framesReq > framesAvail) {
        framesReq = framesAvail;
    }

    uint32_t u = cblk->user;
    uint32_t bufferEnd = cblk->userBase + cblk->frameCount;

    if (framesReq > bufferEnd - u) {
        framesReq = bufferEnd - u;
    }

    audioBuffer->flags = mMuted ? Buffer::MUTE : 0;
    audioBuffer->channelCount = mChannelCount;
    audioBuffer->frameCount = framesReq;
    audioBuffer->size = framesReq * cblk->frameSize;
    if (audio_is_linear_pcm(mFormat)) {
        audioBuffer->format = AUDIO_FORMAT_PCM_16_BIT;
    } else {
        audioBuffer->format = mFormat;
    }
    audioBuffer->raw = (int8_t *)cblk->buffer(u);
    active = mActive;
    return active ? status_t(NO_ERROR) : status_t(STOPPED);
}

void AudioTrack::releaseBuffer(Buffer* audioBuffer)
{
    AutoMutex lock(mLock);
    mCblk->stepUser(audioBuffer->frameCount);
    if (audioBuffer->frameCount > 0) {
        // restart track if it was disabled by audioflinger due to previous underrun
        if (mActive && (mCblk->flags & CBLK_DISABLED_MSK)) {
            android_atomic_and(~CBLK_DISABLED_ON, &mCblk->flags);
            ALOGW("releaseBuffer() track %p name=%#x disabled, restarting", this, mCblk->mName);
            mAudioTrack->start();
        }
    }
}

// -------------------------------------------------------------------------

ssize_t AudioTrack::write(const void* buffer, size_t userSize)
{

    if (mSharedBuffer != 0) return INVALID_OPERATION;
    if (mIsTimed) return INVALID_OPERATION;

    if (ssize_t(userSize) < 0) {
        // Sanity-check: user is most-likely passing an error code, and it would
        // make the return value ambiguous (actualSize vs error).
        ALOGE("AudioTrack::write(buffer=%p, size=%u (%d)",
                buffer, userSize, userSize);
        return BAD_VALUE;
    }

    ALOGV("write %p: %d bytes, mActive=%d", this, userSize, mActive);

    if (userSize == 0) {
        return 0;
    }

    // acquire a strong reference on the IMemory and IAudioTrack so that they cannot be destroyed
    // while we are accessing the cblk
    mLock.lock();
    sp<IAudioTrack> audioTrack = mAudioTrack;
    sp<IMemory> iMem = mCblkMemory;
    mLock.unlock();

    ssize_t written = 0;
    const int8_t *src = (const int8_t *)buffer;
    Buffer audioBuffer;
    size_t frameSz = frameSize();

    do {
        audioBuffer.frameCount = userSize/frameSz;

        status_t err = obtainBuffer(&audioBuffer, -1);
        if (err < 0) {
            // out of buffers, return #bytes written
            if (err == status_t(NO_MORE_BUFFERS))
                break;
            return ssize_t(err);
        }

        size_t toWrite;

        if (mFormat == AUDIO_FORMAT_PCM_8_BIT && !(mFlags & AUDIO_OUTPUT_FLAG_DIRECT)) {
            // Divide capacity by 2 to take expansion into account
            toWrite = audioBuffer.size>>1;
            memcpy_to_i16_from_u8(audioBuffer.i16, (const uint8_t *) src, toWrite);
        } else {
            toWrite = audioBuffer.size;
            memcpy(audioBuffer.i8, src, toWrite);
            src += toWrite;
        }
        userSize -= toWrite;
        written += toWrite;

        releaseBuffer(&audioBuffer);
    } while (userSize >= frameSz);

    return written;
}

// -------------------------------------------------------------------------

TimedAudioTrack::TimedAudioTrack() {
    mIsTimed = true;
}

status_t TimedAudioTrack::allocateTimedBuffer(size_t size, sp<IMemory>* buffer)
{
    status_t result = UNKNOWN_ERROR;

    // If the track is not invalid already, try to allocate a buffer.  alloc
    // fails indicating that the server is dead, flag the track as invalid so
    // we can attempt to restore in just a bit.
    if (!(mCblk->flags & CBLK_INVALID_MSK)) {
        result = mAudioTrack->allocateTimedBuffer(size, buffer);
        if (result == DEAD_OBJECT) {
            android_atomic_or(CBLK_INVALID_ON, &mCblk->flags);
        }
    }

    // If the track is invalid at this point, attempt to restore it. and try the
    // allocation one more time.
    if (mCblk->flags & CBLK_INVALID_MSK) {
        mCblk->lock.lock();
        result = restoreTrack_l(mCblk, false);
        mCblk->lock.unlock();

        if (result == OK)
            result = mAudioTrack->allocateTimedBuffer(size, buffer);
    }

    return result;
}

status_t TimedAudioTrack::queueTimedBuffer(const sp<IMemory>& buffer,
                                           int64_t pts)
{
    status_t status = mAudioTrack->queueTimedBuffer(buffer, pts);
    {
        AutoMutex lock(mLock);
        // restart track if it was disabled by audioflinger due to previous underrun
        if (buffer->size() != 0 && status == NO_ERROR &&
                mActive && (mCblk->flags & CBLK_DISABLED_MSK)) {
            android_atomic_and(~CBLK_DISABLED_ON, &mCblk->flags);
            ALOGW("queueTimedBuffer() track %p disabled, restarting", this);
            mAudioTrack->start();
        }
    }
    return status;
}

status_t TimedAudioTrack::setMediaTimeTransform(const LinearTransform& xform,
                                                TargetTimeline target)
{
    return mAudioTrack->setMediaTimeTransform(xform, target);
}

// -------------------------------------------------------------------------

bool AudioTrack::processAudioBuffer(const sp<AudioTrackThread>& thread)
{
    Buffer audioBuffer;
    uint32_t frames;
    size_t writtenSize;

    mLock.lock();
    // acquire a strong reference on the IMemory and IAudioTrack so that they cannot be destroyed
    // while we are accessing the cblk
    sp<IAudioTrack> audioTrack = mAudioTrack;
    sp<IMemory> iMem = mCblkMemory;
    audio_track_cblk_t* cblk = mCblk;
    bool active = mActive;
    mLock.unlock();

    // Manage underrun callback
    if (active && (cblk->framesAvailable() == cblk->frameCount)) {
        ALOGV("Underrun user: %x, server: %x, flags %04x", cblk->user, cblk->server, cblk->flags);
        if (!(android_atomic_or(CBLK_UNDERRUN_ON, &cblk->flags) & CBLK_UNDERRUN_MSK)) {
            mCbf(EVENT_UNDERRUN, mUserData, 0);
            if (cblk->server == cblk->frameCount) {
                mCbf(EVENT_BUFFER_END, mUserData, 0);
            }
            if (mSharedBuffer != 0) return false;
        }
    }

    // Manage loop end callback
    while (mLoopCount > cblk->loopCount) {
        int loopCount = -1;
        mLoopCount--;
        if (mLoopCount >= 0) loopCount = mLoopCount;

        mCbf(EVENT_LOOP_END, mUserData, (void *)&loopCount);
    }

    // Manage marker callback
    if (!mMarkerReached && (mMarkerPosition > 0)) {
        if (cblk->server >= mMarkerPosition) {
            mCbf(EVENT_MARKER, mUserData, (void *)&mMarkerPosition);
            mMarkerReached = true;
        }
    }

    // Manage new position callback
    if (mUpdatePeriod > 0) {
        while (cblk->server >= mNewPosition) {
            mCbf(EVENT_NEW_POS, mUserData, (void *)&mNewPosition);
            mNewPosition += mUpdatePeriod;
        }
    }

    // If Shared buffer is used, no data is requested from client.
    if (mSharedBuffer != 0) {
        frames = 0;
    } else {
        frames = mRemainingFrames;
    }

    // See description of waitCount parameter at declaration of obtainBuffer().
    // The logic below prevents us from being stuck below at obtainBuffer()
    // not being able to handle timed events (position, markers, loops).
    int32_t waitCount = -1;
    if (mUpdatePeriod || (!mMarkerReached && mMarkerPosition) || mLoopCount) {
        waitCount = 1;
    }

    do {

        audioBuffer.frameCount = frames;

        status_t err = obtainBuffer(&audioBuffer, waitCount);
        if (err < NO_ERROR) {
            if (err != TIMED_OUT) {
                ALOGE_IF(err != status_t(NO_MORE_BUFFERS), "Error obtaining an audio buffer, giving up.");
                return false;
            }
            break;
        }
        if (err == status_t(STOPPED)) return false;

        // Divide buffer size by 2 to take into account the expansion
        // due to 8 to 16 bit conversion: the callback must fill only half
        // of the destination buffer
        if (mFormat == AUDIO_FORMAT_PCM_8_BIT && !(mFlags & AUDIO_OUTPUT_FLAG_DIRECT)) {
            audioBuffer.size >>= 1;
        }

        size_t reqSize = audioBuffer.size;
        mCbf(EVENT_MORE_DATA, mUserData, &audioBuffer);
        writtenSize = audioBuffer.size;

        // Sanity check on returned size
        if (ssize_t(writtenSize) <= 0) {
            // The callback is done filling buffers
            // Keep this thread going to handle timed events and
            // still try to get more data in intervals of WAIT_PERIOD_MS
            // but don't just loop and block the CPU, so wait
            usleep(WAIT_PERIOD_MS*1000);
            break;
        }

        if (writtenSize > reqSize) writtenSize = reqSize;

        if (mFormat == AUDIO_FORMAT_PCM_8_BIT && !(mFlags & AUDIO_OUTPUT_FLAG_DIRECT)) {
            // 8 to 16 bit conversion, note that source and destination are the same address
            memcpy_to_i16_from_u8(audioBuffer.i16, (const uint8_t *) audioBuffer.i8, writtenSize);
            writtenSize <<= 1;
        }

        audioBuffer.size = writtenSize;
        // NOTE: mCblk->frameSize is not equal to AudioTrack::frameSize() for
        // 8 bit PCM data: in this case,  mCblk->frameSize is based on a sample size of
        // 16 bit.
        audioBuffer.frameCount = writtenSize/mCblk->frameSize;

        frames -= audioBuffer.frameCount;

        releaseBuffer(&audioBuffer);
    }
    while (frames);

    if (frames == 0) {
        mRemainingFrames = mNotificationFramesAct;
    } else {
        mRemainingFrames = frames;
    }
    return true;
}

// must be called with mLock and cblk.lock held. Callers must also hold strong references on
// the IAudioTrack and IMemory in case they are recreated here.
// If the IAudioTrack is successfully restored, the cblk pointer is updated
status_t AudioTrack::restoreTrack_l(audio_track_cblk_t*& cblk, bool fromStart)
{
    status_t result;

    if (!(android_atomic_or(CBLK_RESTORING_ON, &cblk->flags) & CBLK_RESTORING_MSK)) {
        ALOGW("dead IAudioTrack, creating a new one from %s TID %d",
            fromStart ? "start()" : "obtainBuffer()", gettid());

        // signal old cblk condition so that other threads waiting for available buffers stop
        // waiting now
        cblk->cv.broadcast();
        cblk->lock.unlock();

        // refresh the audio configuration cache in this process to make sure we get new
        // output parameters in getOutput_l() and createTrack_l()
        AudioSystem::clearAudioConfigCache();

        // if the new IAudioTrack is created, createTrack_l() will modify the
        // following member variables: mAudioTrack, mCblkMemory and mCblk.
        // It will also delete the strong references on previous IAudioTrack and IMemory
        result = createTrack_l(mStreamType,
                               cblk->sampleRate,
                               mFormat,
                               mChannelMask,
                               mFrameCount,
                               mFlags,
                               mSharedBuffer,
                               getOutput_l());

        if (result == NO_ERROR) {
            uint32_t user = cblk->user;
            uint32_t server = cblk->server;
            // restore write index and set other indexes to reflect empty buffer status
            mCblk->user = user;
            mCblk->server = user;
            mCblk->userBase = user;
            mCblk->serverBase = user;
            // restore loop: this is not guaranteed to succeed if new frame count is not
            // compatible with loop length
            setLoop_l(cblk->loopStart, cblk->loopEnd, cblk->loopCount);
            if (!fromStart) {
                mCblk->bufferTimeoutMs = MAX_RUN_TIMEOUT_MS;
                // Make sure that a client relying on callback events indicating underrun or
                // the actual amount of audio frames played (e.g SoundPool) receives them.
                if (mSharedBuffer == 0) {
                    uint32_t frames = 0;
                    if (user > server) {
                        frames = ((user - server) > mCblk->frameCount) ?
                                mCblk->frameCount : (user - server);
                        memset(mCblk->buffers, 0, frames * mCblk->frameSize);
                    }
                    // restart playback even if buffer is not completely filled.
                    android_atomic_or(CBLK_FORCEREADY_ON, &mCblk->flags);
                    // stepUser() clears CBLK_UNDERRUN_ON flag enabling underrun callbacks to
                    // the client
                    mCblk->stepUser(frames);
                }
            }
            if (mSharedBuffer != 0) {
                mCblk->stepUser(mCblk->frameCount);
            }
            if (mActive) {
                result = mAudioTrack->start();
                ALOGW_IF(result != NO_ERROR, "restoreTrack_l() start() failed status %d", result);
            }
            if (fromStart && result == NO_ERROR) {
                mNewPosition = mCblk->server + mUpdatePeriod;
            }
        }
        if (result != NO_ERROR) {
            android_atomic_and(~CBLK_RESTORING_ON, &cblk->flags);
            ALOGW_IF(result != NO_ERROR, "restoreTrack_l() failed status %d", result);
        }
        mRestoreStatus = result;
        // signal old cblk condition for other threads waiting for restore completion
        android_atomic_or(CBLK_RESTORED_ON, &cblk->flags);
        cblk->cv.broadcast();
    } else {
        if (!(cblk->flags & CBLK_RESTORED_MSK)) {
            ALOGW("dead IAudioTrack, waiting for a new one TID %d", gettid());
            mLock.unlock();
            result = cblk->cv.waitRelative(cblk->lock, milliseconds(RESTORE_TIMEOUT_MS));
            if (result == NO_ERROR) {
                result = mRestoreStatus;
            }
            cblk->lock.unlock();
            mLock.lock();
        } else {
            ALOGW("dead IAudioTrack, already restored TID %d", gettid());
            result = mRestoreStatus;
            cblk->lock.unlock();
        }
    }
    ALOGV("restoreTrack_l() status %d mActive %d cblk %p, old cblk %p flags %08x old flags %08x",
        result, mActive, mCblk, cblk, mCblk->flags, cblk->flags);

    if (result == NO_ERROR) {
        // from now on we switch to the newly created cblk
        cblk = mCblk;
    }
    cblk->lock.lock();

    ALOGW_IF(result != NO_ERROR, "restoreTrack_l() error %d TID %d", result, gettid());

    return result;
}

status_t AudioTrack::dump(int fd, const Vector<String16>& args) const
{

    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    result.append(" AudioTrack::dump\n");
    snprintf(buffer, 255, "  stream type(%d), left - right volume(%f, %f)\n", mStreamType, mVolume[0], mVolume[1]);
    result.append(buffer);
    snprintf(buffer, 255, "  format(%d), channel count(%d), frame count(%d)\n", mFormat, mChannelCount, mCblk->frameCount);
    result.append(buffer);
    snprintf(buffer, 255, "  sample rate(%d), status(%d), muted(%d)\n", (mCblk == 0) ? 0 : mCblk->sampleRate, mStatus, mMuted);
    result.append(buffer);
    snprintf(buffer, 255, "  active(%d), latency (%d)\n", mActive, mLatency);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

// =========================================================================

AudioTrack::AudioTrackThread::AudioTrackThread(AudioTrack& receiver, bool bCanCallJava)
    : Thread(bCanCallJava), mReceiver(receiver), mPaused(true)
{
}

AudioTrack::AudioTrackThread::~AudioTrackThread()
{
}

bool AudioTrack::AudioTrackThread::threadLoop()
{
    {
        AutoMutex _l(mMyLock);
        if (mPaused) {
            mMyCond.wait(mMyLock);
            // caller will check for exitPending()
            return true;
        }
    }
    if (!mReceiver.processAudioBuffer(this)) {
        pause();
    }
    return true;
}

void AudioTrack::AudioTrackThread::requestExit()
{
    // must be in this order to avoid a race condition
    Thread::requestExit();
    resume();
}

void AudioTrack::AudioTrackThread::pause()
{
    AutoMutex _l(mMyLock);
    mPaused = true;
}

void AudioTrack::AudioTrackThread::resume()
{
    AutoMutex _l(mMyLock);
    if (mPaused) {
        mPaused = false;
        mMyCond.signal();
    }
}

// =========================================================================


audio_track_cblk_t::audio_track_cblk_t()
    : lock(Mutex::SHARED), cv(Condition::SHARED), user(0), server(0),
    userBase(0), serverBase(0), buffers(NULL), frameCount(0),
    loopStart(UINT_MAX), loopEnd(UINT_MAX), loopCount(0), mVolumeLR(0x10001000),
    mSendLevel(0), flags(0)
{
}

uint32_t audio_track_cblk_t::stepUser(uint32_t frameCount)
{
    ALOGV("stepuser %08x %08x %d", user, server, frameCount);

    uint32_t u = user;
    u += frameCount;
    // Ensure that user is never ahead of server for AudioRecord
    if (flags & CBLK_DIRECTION_MSK) {
        // If stepServer() has been called once, switch to normal obtainBuffer() timeout period
        if (bufferTimeoutMs == MAX_STARTUP_TIMEOUT_MS-1) {
            bufferTimeoutMs = MAX_RUN_TIMEOUT_MS;
        }
    } else if (u > server) {
        ALOGW("stepUser occurred after track reset");
        u = server;
    }

    uint32_t fc = this->frameCount;
    if (u >= fc) {
        // common case, user didn't just wrap
        if (u - fc >= userBase ) {
            userBase += fc;
        }
    } else if (u >= userBase + fc) {
        // user just wrapped
        userBase += fc;
    }

    user = u;

    // Clear flow control error condition as new data has been written/read to/from buffer.
    if (flags & CBLK_UNDERRUN_MSK) {
        android_atomic_and(~CBLK_UNDERRUN_MSK, &flags);
    }

    return u;
}

bool audio_track_cblk_t::stepServer(uint32_t frameCount)
{
    ALOGV("stepserver %08x %08x %d", user, server, frameCount);

    if (!tryLock()) {
        ALOGW("stepServer() could not lock cblk");
        return false;
    }

    uint32_t s = server;
    bool flushed = (s == user);

    s += frameCount;
    if (flags & CBLK_DIRECTION_MSK) {
        // Mark that we have read the first buffer so that next time stepUser() is called
        // we switch to normal obtainBuffer() timeout period
        if (bufferTimeoutMs == MAX_STARTUP_TIMEOUT_MS) {
            bufferTimeoutMs = MAX_STARTUP_TIMEOUT_MS - 1;
        }
        // It is possible that we receive a flush()
        // while the mixer is processing a block: in this case,
        // stepServer() is called After the flush() has reset u & s and
        // we have s > u
        if (flushed) {
            ALOGW("stepServer occurred after track reset");
            s = user;
        }
    }

    if (s >= loopEnd) {
        ALOGW_IF(s > loopEnd, "stepServer: s %u > loopEnd %u", s, loopEnd);
        s = loopStart;
        if (--loopCount == 0) {
            loopEnd = UINT_MAX;
            loopStart = UINT_MAX;
        }
    }

    uint32_t fc = this->frameCount;
    if (s >= fc) {
        // common case, server didn't just wrap
        if (s - fc >= serverBase ) {
            serverBase += fc;
        }
    } else if (s >= serverBase + fc) {
        // server just wrapped
        serverBase += fc;
    }

    server = s;

    if (!(flags & CBLK_INVALID_MSK)) {
        cv.signal();
    }
    lock.unlock();
    return true;
}

void* audio_track_cblk_t::buffer(uint32_t offset) const
{
    return (int8_t *)buffers + (offset - userBase) * frameSize;
}

uint32_t audio_track_cblk_t::framesAvailable()
{
    Mutex::Autolock _l(lock);
    return framesAvailable_l();
}

uint32_t audio_track_cblk_t::framesAvailable_l()
{
    uint32_t u = user;
    uint32_t s = server;

    if (flags & CBLK_DIRECTION_MSK) {
        uint32_t limit = (s < loopStart) ? s : loopStart;
        return limit + frameCount - u;
    } else {
        return frameCount + u - s;
    }
}

uint32_t audio_track_cblk_t::framesReady()
{
    uint32_t u = user;
    uint32_t s = server;

    if (flags & CBLK_DIRECTION_MSK) {
        if (u < loopEnd) {
            return u - s;
        } else {
            // do not block on mutex shared with client on AudioFlinger side
            if (!tryLock()) {
                ALOGW("framesReady() could not lock cblk");
                return 0;
            }
            uint32_t frames = UINT_MAX;
            if (loopCount >= 0) {
                frames = (loopEnd - loopStart)*loopCount + u - s;
            }
            lock.unlock();
            return frames;
        }
    } else {
        return s - u;
    }
}

bool audio_track_cblk_t::tryLock()
{
    // the code below simulates lock-with-timeout
    // we MUST do this to protect the AudioFlinger server
    // as this lock is shared with the client.
    status_t err;

    err = lock.tryLock();
    if (err == -EBUSY) { // just wait a bit
        usleep(1000);
        err = lock.tryLock();
    }
    if (err != NO_ERROR) {
        // probably, the client just died.
        return false;
    }
    return true;
}

// -------------------------------------------------------------------------

}; // namespace android

/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_AUDIO_RESAMPLER_H
#define ANDROID_AUDIO_RESAMPLER_H

#include <stdint.h>
#include <sys/types.h>

#include <media/AudioBufferProvider.h>

namespace android {
// ----------------------------------------------------------------------------

class AudioResampler {
public:
    // Determines quality of SRC.
    //  LOW_QUALITY: linear interpolator (1st order)
    //  MED_QUALITY: cubic interpolator (3rd order)
    //  HIGH_QUALITY: fixed multi-tap FIR (e.g. 48KHz->44.1KHz)
    // NOTE: high quality SRC will only be supported for
    // certain fixed rate conversions. Sample rate cannot be
    // changed dynamically.
    enum src_quality {
        DEFAULT_QUALITY=0,
        LOW_QUALITY=1,
        MED_QUALITY=2,
        HIGH_QUALITY=3,
        VERY_HIGH_QUALITY=4,
    };

    static AudioResampler* create(int bitDepth, int inChannelCount,
            int32_t sampleRate, src_quality quality=DEFAULT_QUALITY);

    virtual ~AudioResampler();

    virtual void init() = 0;
    virtual void setSampleRate(int32_t inSampleRate);
    virtual void setVolume(int16_t left, int16_t right);
    virtual void setLocalTimeFreq(uint64_t freq);

    // set the PTS of the next buffer output by the resampler
    virtual void setPTS(int64_t pts);

    virtual void resample(int32_t* out, size_t outFrameCount,
            AudioBufferProvider* provider) = 0;

    virtual void reset();
    virtual size_t getUnreleasedFrames() const { return mInputIndex; }

    // called from destructor, so must not be virtual
    src_quality getQuality() const { return mQuality; }

protected:
    // number of bits for phase fraction - 30 bits allows nearly 2x downsampling
    static const int kNumPhaseBits = 30;

    // phase mask for fraction
    static const uint32_t kPhaseMask = (1LU<<kNumPhaseBits)-1;

    // multiplier to calculate fixed point phase increment
    static const double kPhaseMultiplier = 1L << kNumPhaseBits;

    AudioResampler(int bitDepth, int inChannelCount, int32_t sampleRate, src_quality quality);

    // prevent copying
    AudioResampler(const AudioResampler&);
    AudioResampler& operator=(const AudioResampler&);

    int64_t calculateOutputPTS(int outputFrameIndex);

    const int32_t mBitDepth;
    const int32_t mChannelCount;
    const int32_t mSampleRate;
    int32_t mInSampleRate;
    AudioBufferProvider::Buffer mBuffer;
    union {
        int16_t mVolume[2];
        uint32_t mVolumeRL;
    };
    int16_t mTargetVolume[2];
    size_t mInputIndex;
    int32_t mPhaseIncrement;
    uint32_t mPhaseFraction;
    uint64_t mLocalTimeFreq;
    int64_t mPTS;

private:
    const src_quality mQuality;

    // Return 'true' if the quality level is supported without explicit request
    static bool qualityIsSupported(src_quality quality);

    // For pthread_once()
    static void init_routine();

    // Return the estimated CPU load for specific resampler in MHz.
    // The absolute number is irrelevant, it's the relative values that matter.
    static uint32_t qualityMHz(src_quality quality);
};

// ----------------------------------------------------------------------------
}
; // namespace android

#endif // ANDROID_AUDIO_RESAMPLER_H

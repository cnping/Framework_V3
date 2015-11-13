/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef ANDROID_AUDIO_STATE_QUEUE_H
#define ANDROID_AUDIO_STATE_QUEUE_H

namespace android {

#ifdef STATE_QUEUE_DUMP
// The StateQueueObserverDump and StateQueueMutatorDump keep
// a cache of StateQueue statistics that can be logged by dumpsys.
// Each individual native word-sized field is accessed atomically.  But the
// overall structure is non-atomic, that is there may be an inconsistency between fields.
// No barriers or locks are used for either writing or reading.
// Only POD types are permitted, and the contents shouldn't be trusted (i.e. do range checks).
// It has a different lifetime than the StateQueue, and so it can't be a member of StateQueue.

struct StateQueueObserverDump {
    StateQueueObserverDump() : mStateChanges(0) { }
    /*virtual*/ ~StateQueueObserverDump() { }
    unsigned    mStateChanges;    // incremented each time poll() detects a state change
    void        dump(int fd);
};

struct StateQueueMutatorDump {
    StateQueueMutatorDump() : mPushDirty(0), mPushAck(0), mBlockedSequence(0) { }
    /*virtual*/ ~StateQueueMutatorDump() { }
    unsigned    mPushDirty;       // incremented each time push() is called with a dirty state
    unsigned    mPushAck;         // incremented each time push(BLOCK_UNTIL_ACKED) is called
    unsigned    mBlockedSequence; // incremented before and after each time that push()
                                  // blocks for more than one PUSH_BLOCK_ACK_NS;
                                  // if odd, then mutator is currently blocked inside push()
    void        dump(int fd);
};
#endif

// manages a FIFO queue of states
template<typename T> class StateQueue {

public:
            StateQueue();
    virtual ~StateQueue();

    // Observer APIs

    // Poll for a state change.  Returns a pointer to a read-only state,
    // or NULL if the state has not been initialized yet.
    // If a new state has not pushed by mutator since the previous poll,
    // then the returned pointer will be unchanged.
    // The previous state pointer is guaranteed to still be valid;
    // this allows the observer to diff the previous and new states.
    const T* poll();

    // Mutator APIs

    // Begin a mutation.  Returns a pointer to a read/write state, except the
    // first time it is called the state is write-only and _must_ be initialized.
    // Mutations cannot be nested.
    // If the state is dirty and has not been pushed onto the state queue yet, then
    // this new mutation will be squashed together with the previous one.
    T*      begin();

    // End the current mutation and indicate whether caller modified the state.
    // If didModify is true, then the state is marked dirty (in need of pushing).
    // There is no rollback option because modifications are done in place.
    // Does not automatically push the new state onto the state queue.
    void    end(bool didModify = true);

    // Push a new state, if any, out to the observer via the state queue.
    // For BLOCK_NEVER, returns:
    //      true if not dirty, or dirty and pushed successfully
    //      false if dirty and not pushed because that would block; remains dirty
    // For BLOCK_UNTIL_PUSHED and BLOCK_UNTIL_ACKED, always returns true.
    // No-op if there are no pending modifications (not dirty), except
    //      for BLOCK_UNTIL_ACKED it will wait until a prior push has been acknowledged.
    // Must not be called in the middle of a mutation.
    enum block_t {
        BLOCK_NEVER,        // do not block
        BLOCK_UNTIL_PUSHED, // block until there's a slot available for the push
        BLOCK_UNTIL_ACKED,  // also block until the push is acknowledged by the observer
    };
    bool    push(block_t block = BLOCK_NEVER);

    // Return whether the current state is dirty (modified and not pushed).
    bool    isDirty() const { return mIsDirty; }

#ifdef STATE_QUEUE_DUMP
    // Register location of observer dump area
    void    setObserverDump(StateQueueObserverDump *dump)
            { mObserverDump = dump != NULL ? dump : &mObserverDummyDump; }

    // Register location of mutator dump area
    void    setMutatorDump(StateQueueMutatorDump *dump)
            { mMutatorDump = dump != NULL ? dump : &mMutatorDummyDump; }
#endif

private:
    static const unsigned kN = 4;       // values != 4 are not supported by this code
    T                 mStates[kN];      // written by mutator, read by observer

    // "volatile" is meaningless with SMP, but here it indicates that we're using atomic ops
    volatile const T* mNext; // written by mutator to advance next, read by observer
    volatile const T* mAck;  // written by observer to acknowledge advance of next, read by mutator

    // only used by observer
    const T*          mCurrent;         // most recent value returned by poll()

    // only used by mutator
    T*                mMutating;        // where updates by mutator are done in place
    const T*          mExpecting;       // what the mutator expects mAck to be set to
    bool              mInMutation;      // whether we're currently in the middle of a mutation
    bool              mIsDirty;         // whether mutating state has been modified since last push
    bool              mIsInitialized;   // whether mutating state has been initialized yet

#ifdef STATE_QUEUE_DUMP
    StateQueueObserverDump  mObserverDummyDump; // default area for observer dump if not set
    StateQueueObserverDump* mObserverDump;      // pointer to active observer dump, always non-NULL
    StateQueueMutatorDump   mMutatorDummyDump;  // default area for mutator dump if not set
    StateQueueMutatorDump*  mMutatorDump;       // pointer to active mutator dump, always non-NULL
#endif

};  // class StateQueue

}   // namespace android

#endif  // ANDROID_AUDIO_STATE_QUEUE_H

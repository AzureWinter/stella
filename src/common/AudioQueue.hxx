//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2018 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef AUDIO_QUEUE_HXX
#define AUDIO_QUEUE_HXX

#include <mutex>

#include "bspf.hxx"

/**
   This class implements a an audio queue that acts both like a ring buffer
   and a pool of audio fragments. The TIA emulation core fills a fragment
   with samples and then returns it to the queue, receiving a new fragment
   in return. The sound driver removes fragments for playback from the
   queue and returns the used fragment in this process.

   The queue needs to be threadsafe as the (SDL) audio driver runs on a
   separate thread. Samples are stored as signed 16 bit integers
   (platform endian).
 */
class AudioQueue
{
  public:

    /**
       Create a new AudioQueue.

       @param fragmentSize      The size (in stereo / mono samples) of each fragment
       @param capacaity         The number of fragments that can be queued before wrapping.
       @param isStereo          Whether samples are stereo or mono.
       @param sampleRate        The sample rate. This is not used, but can be queried.
     */
    AudioQueue(uInt32 fragmentSize, uInt8 capacity, bool isStereo, uInt16 sampleRate);

    /**
       We need a destructor to deallocate the individual fragment buffers.
     */
    ~AudioQueue();

    /**
       Capacity getter.
     */
    uInt8 capacity() const;

    /**
      Size getter.
     */
    uInt8 size();

    /**
      Stereo / mono getter.
     */
    bool isStereo() const;

    /**
      Fragment size getter.
     */
    uInt32 fragmentSize() const;

    /**
      Sample rate getter.
     */
    uInt16 sampleRate() const;

    /**
      Enqueue a new fragment and get a new fragmen to fill.

      @param fragment   The returned fragment. This must be empty on the first call (when
                        there is nothing to return)
     */
    Int16* enqueue(Int16* fragment = 0);

    /**
     * Dequeue a fragment for playback and return the played fragment. This may
     * return 0 if there is no queued fragment to return (in this case, the returned
     * fragment is not enqueued and must be passed in the next invocation).
     *
     * @param fragment  The returned fragment. This must be empty on the first call (when
     *                  there is nothing to return).
     */
    Int16* dequeue(Int16* fragment = 0);

  private:

    // The size of an individual fragment (in stereo / mono samples)
    uInt32 myFragmentSize;

    // Are we using stereo samples?
    bool myIsStereo;

    // The sample rate
    uInt16 mySampleRate;

    // The fragment queue
    vector<Int16*> myFragmentQueue;

    // All fragments, including the two fragments that are in circulation.
    vector<Int16*> myAllFragments;

    // We allocate a consecutive slice of memory for the fragments.
    Int16* myFragmentBuffer;

    // The nubmer if queued fragments
    uInt8 mySize;

    // The next fragment.
    uInt8 myNextFragment;

    // We need a mutex for thread safety.
    std::mutex myMutex;

    // The first (empty) enqueue call returns this fragment.
    Int16* myFirstFragmentForEnqueue;
    // The first (empty) dequeue call replaces the returned fragment with this fragment.
    Int16* myFirstFragmentForDequeue;

  private:

    AudioQueue() = delete;
    AudioQueue(const AudioQueue&) = delete;
    AudioQueue(AudioQueue&&) = delete;
    AudioQueue& operator=(const AudioQueue&) = delete;
    AudioQueue& operator=(AudioQueue&&) = delete;
};

#endif // AUDIO_QUEUE_HXX

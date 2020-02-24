/*
  ==============================================================================

    SpatialSynthVoice.h
    Created: 19 Feb 2020 10:38:33pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "SpatialSynthSound.h"
#include "vec3.hpp"

//==============================================================================
/**
    Represents a voice that a SpatialSynth can use to play a SpatialSynthSound.

    A voice plays a single sound at a time, and a synthesiser holds an array of
    voices so that it can play polyphonically.

    @see SpatialSynth, SpatialSynthSound

    @tags{Audio}
*/
class SpatialSynthVoice
{
public:
    //==============================================================================
    /** Creates a voice. */
    SpatialSynthVoice();

    /** Destructor. */
    virtual ~SpatialSynthVoice();

    //==============================================================================
    /** Returns the midi note that this voice is currently playing.
        Returns a value less than 0 if no note is playing.
    */
    int getCurrentNoteID() const noexcept                        { return mCurrentNoteID; }

    /** Returns the current target sample rate at which rendering is being done.
        Subclasses may need to know this so that they can pitch things correctly.
    */
    double getSampleRate() const noexcept                       { return mCurrentSampleRate; }

    /** Returns the sound that this voice is currently playing.
        Returns nullptr if it's not playing.
    */
    SpatialSynthSound::Ptr getCurrentlyPlayingSound() const noexcept     { return mCurrentlyPlayingSound; }

    /** Must return true if this voice object is capable of playing the given sound.

        If there are different classes of sound, and different classes of voice, a voice can
        choose which ones it wants to take on.

        A typical implementation of this method may just return true if there's only one type
        of voice and sound, or it might check the type of the sound object passed-in and
        see if it's one that it understands.
    */
    virtual bool canPlaySound (SpatialSynthSound*) = 0;

    /** Called to start a new note.
        This will be called during the rendering callback, so must be fast and thread-safe.
    */
    virtual void startNote (int noteID,
                            float velocity,
                            const glm::vec3& pos,
                            SpatialSynthSound* sound) = 0;

    /** Called to stop a note.

        This will be called during the rendering callback, so must be fast and thread-safe.

        The velocity indicates how quickly the note was released - 0 is slowly, 1 is quickly.

        If allowTailOff is false or the voice doesn't want to tail-off, then it must stop all
        sound immediately, and must call clearCurrentNote() to reset the state of this voice
        and allow the synth to reassign it another sound.

        If allowTailOff is true and the voice decides to do a tail-off, then it's allowed to
        begin fading out its sound, and it can stop playing until it's finished. As soon as it
        finishes playing (during the rendering callback), it must make sure that it calls
        clearCurrentNote().
    */
    virtual void stopNote (float velocity, bool allowTailOff) = 0;

    /** Returns true if this voice is currently busy playing a sound.
        By default this just checks the getCurrentlyPlayingNote() value, but can
        be overridden for more advanced checking.
    */
    virtual bool isVoiceActive() const;

    /** Called to let the voice know that the pitch wheel has been moved.
        This will be called during the rendering callback, so must be fast and thread-safe.
    */
    void positionChanged (const glm::vec3& newPosition);


    //==============================================================================
    /** Renders the next block of data for this voice.

        The output audio data must be added to the current contents of the buffer provided.
        Only the region of the buffer between startSample and (startSample + numSamples)
        should be altered by this method.

        If the voice is currently silent, it should just return without doing anything.

        If the sound that the voice is playing finishes during the course of this rendered
        block, it must call clearCurrentNote(), to tell the synthesiser that it has finished.

        The size of the blocks that are rendered can change each time it is called, and may
        involve rendering as little as 1 sample at a time. In between rendering callbacks,
        the voice's methods will be called to tell it about note and controller events.
    */
    virtual void renderNextBlock (AudioBuffer<float>& outputBuffer,
                                  int startSample,
                                  int numSamples) = 0;

    /** A double-precision version of renderNextBlock() */
    virtual void renderNextBlock (AudioBuffer<double>& outputBuffer,
                                  int startSample,
                                  int numSamples);

    /** Changes the voice's reference sample rate.

        The rate is set so that subclasses know the output rate and can set their pitch
        accordingly.

        This method is called by the synth, and subclasses can access the current rate with
        the currentSampleRate member.
    */
    virtual void setCurrentPlaybackSampleRate(double newRate);
    
    void setNumSpeakerOutputs(int numSpeakers);
    
    bool getNeedsDBAPUpdate() const { return mNeedsDBAPUpdate; }

    /** Returns true if this voice started playing its current note before the other voice did. */
    bool wasStartedBefore (const SpatialSynthVoice& other) const noexcept;

protected:
    /** Resets the state of this voice after a sound has finished playing.

        The subclass must call this when it finishes playing a note and becomes available
        to play new ones.

        It must either call it in the stopNote() method, or if the voice is tailing off,
        then it should call it later during the renderNextBlock method, as soon as it
        finishes its tail-off.

        It can also be called at any time during the render callback if the sound happens
        to have finished, e.g. if it's playing a sample and the sample finishes.
    */
    void clearCurrentNote();
    
    void updateDBAPAmplitudes(const std::vector<glm::vec3>& positions);

    int                mCurrentNoteID = -1;
    std::vector<float> mChannelAmplitudes;
    std::vector<float> mChannelAmplitudeTargets;
    std::vector<float> mChannelAmplitudeIncrements;
    glm::vec3          mPosition;
    bool               mNeedsDBAPUpdate = true;

private:
    //==============================================================================
    friend class SpatialSynth;

    double                  mCurrentSampleRate = 44100.0;
    uint32                  mNoteOnTime = 0;
    SpatialSynthSound::Ptr  mCurrentlyPlayingSound;
    AudioBuffer<float>      mTempBuffer;

    JUCE_LEAK_DETECTOR (SpatialSynthVoice)
};

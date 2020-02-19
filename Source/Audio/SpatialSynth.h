#pragma once

#include <JuceHeader.h>

#include "SpatialSynthSound.h"
#include "SpatialSynthVoice.h"

/** These classes are based on the Juce Synth classes
    but replace the midi functionality for arbitrary 
    control sources and prioritise multichannel output.
*/

//==============================================================================
/**
    Base class for a musical device that can play sounds.

    To create a synthesiser, you'll need to create a subclass of SpatialSynthSound
    to describe each sound available to your synth, and a subclass of SpatialSynthVoice
    which can play back one of these sounds.

    Then you can use the addVoice() and addSound() methods to give the synthesiser a
    set of sounds, and a set of voices it can use to play them. If you only give it
    one voice it will be monophonic - the more voices it has, the more polyphony it'll
    have available.

    Then repeatedly call the renderNextBlock() method to produce the audio. Any midi
    events that go in will be scanned for note on/off messages, and these are used to
    start and stop the voices playing the appropriate sounds.

    While it's playing, you can also cause notes to be triggered by calling the noteOn(),
    noteOff() and other controller methods.

    Before rendering, be sure to call the setCurrentPlaybackSampleRate() to tell it
    what the target playback rate is. This value is passed on to the voices so that
    they can pitch their output correctly.

    @tags{Audio}
*/
class SpatialSynth
{
public:
    //==============================================================================
    /** Creates a new synthesiser.
        You'll need to add some sounds and voices before it'll make any sound.
    */
    SpatialSynth();

    /** Destructor. */
    virtual ~SpatialSynth();

    //==============================================================================
    /** Deletes all voices. */
    void clearVoices();

    /** Returns the number of voices that have been added. */
    int getNumVoices() const noexcept                               { return voices.size(); }

    /** Returns one of the voices that have been added. */
    SpatialSynthVoice* getVoice (int index) const;

    /** Adds a new voice to the synth.

        All the voices should be the same class of object and are treated equally.

        The object passed in will be managed by the synthesiser, which will delete
        it later on when no longer needed. The caller should not retain a pointer to the
        voice.
    */
    SpatialSynthVoice* addVoice (SpatialSynthVoice* newVoice);

    /** Deletes one of the voices. */
    void removeVoice (int index);

    //==============================================================================
    /** Deletes all sounds. */
    void clearSounds();

    /** Returns the number of sounds that have been added to the synth. */
    int getNumSounds() const noexcept                               { return sounds.size(); }

    /** Returns one of the sounds. */
    SpatialSynthSound::Ptr getSound (int index) const noexcept       { return sounds[index]; }

    /** Adds a new sound to the synthesiser.

        The object passed in is reference counted, so will be deleted when the
        synthesiser and all voices are no longer using it.
    */
    SpatialSynthSound* addSound (const SpatialSynthSound::Ptr& newSound);

    /** Removes and deletes one of the sounds. */
    void removeSound (int index);

    //==============================================================================
    /** If set to true, then the synth will try to take over an existing voice if
        it runs out and needs to play another note.

        The value of this boolean is passed into findFreeVoice(), so the result will
        depend on the implementation of this method.
    */
    void setNoteStealingEnabled (bool shouldStealNotes);

    /** Returns true if note-stealing is enabled.
        @see setNoteStealingEnabled
    */
    bool isNoteStealingEnabled() const noexcept                     { return shouldStealNotes; }

    //==============================================================================
    /** Triggers a note-on event.

        The default method here will find all the sounds that want to be triggered by
        this note/channel. For each sound, it'll try to find a free voice, and use the
        voice to start playing the sound.

        Subclasses might want to override this if they need a more complex algorithm.

        This method will be called automatically according to the midi data passed into
        renderNextBlock(), but may be called explicitly too.

        The midiChannel parameter is the channel, between 1 and 16 inclusive.
    */
    virtual void noteOn (int midiNoteNumber,
                         float velocity,
                         const glm::vec3& pos);

    /** Triggers a note-off event.

        This will turn off any voices that are playing a sound for the given note/channel.

        If allowTailOff is true, the voices will be allowed to fade out the notes gracefully
        (if they can do). If this is false, the notes will all be cut off immediately.

        This method will be called automatically according to the midi data passed into
        renderNextBlock(), but may be called explicitly too.

        The midiChannel parameter is the channel, between 1 and 16 inclusive.
    */
    virtual void noteOff (int midiNoteNumber,
                          float velocity,
                          bool allowTailOff);

    /** Turns off all notes.

        This will turn off any voices that are playing a sound on the given midi channel.

        If midiChannel is 0 or less, then all voices will be turned off, regardless of
        which channel they're playing. Otherwise it represents a valid midi channel, from
        1 to 16 inclusive.

        If allowTailOff is true, the voices will be allowed to fade out the notes gracefully
        (if they can do). If this is false, the notes will all be cut off immediately.

        This method will be called automatically according to the midi data passed into
        renderNextBlock(), but may be called explicitly too.
    */
    virtual void allNotesOff (bool allowTailOff);

    /** Sends a pitch-wheel message to any active voices.

        This will send a pitch-wheel message to any voices that are playing sounds on
        the given midi channel.

        This method will be called automatically according to the midi data passed into
        renderNextBlock(), but may be called explicitly too.

        @param midiChannel          the midi channel, from 1 to 16 inclusive
        @param wheelValue           the wheel position, from 0 to 0x3fff, as returned by MidiMessage::getPitchWheelValue()
    */
    virtual void handlePositionChange (int noteID,
                                      glm::vec3 newPosition);


    //==============================================================================
    /** Tells the synthesiser what the sample rate is for the audio it's being used to render.

        This value is propagated to the voices so that they can use it to render the correct
        pitches.
    */
    virtual void setOutputInfo (int numChannels, double sampleRate);

    /** Creates the next block of audio output.

        This will process the next numSamples of data from all the voices, and add that output
        to the audio block supplied, starting from the offset specified. Note that the
        data will be added to the current contents of the buffer, so you should clear it
        before calling this method if necessary.

        The midi events in the inputMidi buffer are parsed for note and controller events,
        and these are used to trigger the voices. Note that the startSample offset applies
        both to the audio output buffer and the midi input buffer, so any midi events
        with timestamps outside the specified region will be ignored.
    */
    void renderNextBlock (AudioBuffer<float>& outputAudio,
                          int startSample,
                          int numSamples);

    void renderNextBlock (AudioBuffer<double>& outputAudio,
                          int startSample,
                          int numSamples);

    /** Returns the current target sample rate at which rendering is being done.
        Subclasses may need to know this so that they can pitch things correctly.
    */
    double getSampleRate() const noexcept                       { return sampleRate; }

    /** Sets a minimum limit on the size to which audio sub-blocks will be divided when rendering.

        When rendering, the audio blocks that are passed into renderNextBlock() will be split up
        into smaller blocks that lie between all the incoming midi messages, and it is these smaller
        sub-blocks that are rendered with multiple calls to renderVoices().

        Obviously in a pathological case where there are midi messages on every sample, then
        renderVoices() could be called once per sample and lead to poor performance, so this
        setting allows you to set a lower limit on the block size.

        The default setting is 32, which means that midi messages are accurate to about < 1ms
        accuracy, which is probably fine for most purposes, but you may want to increase or
        decrease this value for your synth.

        If shouldBeStrict is true, the audio sub-blocks will strictly never be smaller than numSamples.

        If shouldBeStrict is false (default), the first audio sub-block in the buffer is allowed
        to be smaller, to make sure that the first MIDI event in a buffer will always be sample-accurate
        (this can sometimes help to avoid quantisation or phasing issues).
    */
    void setMinimumRenderingSubdivisionSize (int numSamples, bool shouldBeStrict = false) noexcept;

protected:
    //==============================================================================
    /** This is used to control access to the rendering callback and the note trigger methods. */
    CriticalSection lock;

    OwnedArray<SpatialSynthVoice> voices;
    ReferenceCountedArray<SpatialSynthSound> sounds;

    /** Renders the voices for the given range.
        By default this just calls renderNextBlock() on each voice, but you may need
        to override it to handle custom cases.
    */
    virtual void renderVoices (AudioBuffer<float>& outputAudio,
                               int startSample, int numSamples);
    virtual void renderVoices (AudioBuffer<double>& outputAudio,
                               int startSample, int numSamples);

    /** Searches through the voices to find one that's not currently playing, and
        which can play the given sound.

        Returns nullptr if all voices are busy and stealing isn't enabled.

        To implement a custom note-stealing algorithm, you can either override this
        method, or (preferably) override findVoiceToSteal().
    */
    virtual SpatialSynthVoice* findFreeVoice (SpatialSynthSound* soundToPlay,
                                              int midiNoteNumber,
                                              bool stealIfNoneAvailable) const;

    /** Chooses a voice that is most suitable for being re-used.
        The default method will attempt to find the oldest voice that isn't the
        bottom or top note being played. If that's not suitable for your synth,
        you can override this method and do something more cunning instead.
    */
    virtual SpatialSynthVoice* findVoiceToSteal (SpatialSynthSound* soundToPlay,
                                                 int midiNoteNumber) const;

    /** Starts a specified voice playing a particular sound.
        You'll probably never need to call this, it's used internally by noteOn(), but
        may be needed by subclasses for custom behaviours.
    */
    void startVoice (SpatialSynthVoice* voice,
                     SpatialSynthSound* sound,
                     int midiNoteNumber,
                     float velocity,
                     const glm::vec3& pos);

    /** Stops a given voice.
        You should never need to call this, it's used internally by noteOff, but is protected
        in case it's useful for some custom subclasses. It basically just calls through to
        SpatialSynthVoice::stopNote(), and has some assertions to sanity-check a few things.
    */
    void stopVoice (SpatialSynthVoice*, float velocity, bool allowTailOff);


private:
    //==============================================================================
    double sampleRate = 0;
    uint32 lastNoteOnCounter = 0;
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
    bool shouldStealNotes = true;

    template <typename floatType>
    void processNextBlock (AudioBuffer<floatType>&, int startSample, int numSamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpatialSynth)
};


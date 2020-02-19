
#pragma once

#include <JuceHeader.h>
#include "SpatialSynth.h"

//==============================================================================
/**
    A subclass of SpatialSynthSound that represents a sampled audio clip.

    This is a pretty basic sampler, and just attempts to load the whole audio stream
    into memory.

    To use it, create a SpatialSynth, add some SpatialSamplerVoice objects to it, then
    give it some SampledSound objects to play.

    @see SpatialSamplerVoice, SpatialSynth, SpatialSynthSound

    @tags{Audio}
*/
class SpatialSamplerSound    : public SpatialSynthSound
{
public:
    //==============================================================================
    /** Creates a sampled sound from an audio reader.

        This will attempt to load the audio from the source into memory and store
        it in this object.

        @param name         a name for the sample
        @param source       the audio to load. This object can be safely deleted by the
                            caller after this constructor returns
        @param midiNotes    the set of midi keys that this sound should be played on. This
                            is used by the SpatialSynthSound::appliesToNote() method
        @param midiNoteForNormalPitch   the midi note at which the sample should be played
                                        with its natural rate. All other notes will be pitched
                                        up or down relative to this one
        @param attackTimeSecs   the attack (fade-in) time, in seconds
        @param releaseTimeSecs  the decay (fade-out) time, in seconds
        @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
                                        source, in seconds
    */
    SpatialSamplerSound (const String& name,
                  AudioFormatReader& source,
                  int noteID,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    /** Destructor. */
    ~SpatialSamplerSound() override;

    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept                  { return name; }

    /** Returns the audio sample data.
        This could return nullptr if there was a problem loading the data.
    */
    AudioBuffer<float>* getAudioData() const noexcept       { return data.get(); }

    //==============================================================================
    /** Changes the parameters of the ADSR envelope which will be applied to the sample. */
    void setEnvelopeParameters (ADSR::Parameters parametersToUse)    { params = parametersToUse; }

    //==============================================================================
    bool appliesToNote (int note) override { return note == noteID; };

private:
    //==============================================================================
    friend class SpatialSamplerVoice;

    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    
    int length = 0;
    int noteID = 0;

    ADSR::Parameters params;

    JUCE_LEAK_DETECTOR (SpatialSamplerSound)
};


//==============================================================================
/**
    A subclass of SpatialSynthVoice that can play a SpatialSamplerSound.

    To use it, create a SpatialSynth, add some SpatialSamplerVoice objects to it, then
    give it some SampledSound objects to play.

    @see SpatialSamplerSound, SpatialSynth, SpatialSynthVoice

    @tags{Audio}
*/
class SpatialSamplerVoice    : public SpatialSynthVoice
{
public:
    //==============================================================================
    /** Creates a SpatialSamplerVoice. */
    SpatialSamplerVoice();

    /** Destructor. */
    ~SpatialSamplerVoice() override;

    //==============================================================================
    bool canPlaySound (SpatialSynthSound*) override;

    void startNote (int midiNoteNumber, float velocity, SpatialSynthSound*) override;
    void stopNote (float velocity, bool allowTailOff) override;

    void positionChanged (glm::vec3 newValue) override {}

    void renderNextBlock (AudioBuffer<float>&, int startSample, int numSamples) override;
    using SpatialSynthVoice::renderNextBlock;

private:
    //==============================================================================
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    float lgain = 0, rgain = 0;

    ADSR adsr;

    JUCE_LEAK_DETECTOR (SpatialSamplerVoice)
};

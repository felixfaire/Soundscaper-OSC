
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

*/
class SpatialSamplerSound    : public SpatialSynthSound
{
public:
    //==============================================================================

    SpatialSamplerSound(const String& name,
                  AudioFormatReader& source,
                  int noteID,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    ~SpatialSamplerSound() override;

    //==============================================================================
    
    const String&       getName() const noexcept                  { return mName; }
    AudioBuffer<float>* getAudioData() const noexcept       { return mSampleData.get(); }

    //==============================================================================

    void setEnvelopeParameters(ADSR::Parameters parametersToUse)    { mAdsrParams = parametersToUse; }

    //==============================================================================
    bool appliesToNote(int note) override { return note == mNoteID; };

private:
    //==============================================================================
    friend class SpatialSamplerVoice;

    String                              mName;
    std::unique_ptr<AudioBuffer<float>> mSampleData;
    double                              mSourceSampleRate;
    
    int                                 mLength = 0;
    int                                 mNoteID = 0;

    ADSR::Parameters                    mAdsrParams;

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
    SpatialSamplerVoice();
    ~SpatialSamplerVoice() override;

    //==============================================================================
    bool canPlaySound(SpatialSynthSound*) override;

    void startNote(int midiNoteNumber, float velocity, const glm::vec3& pos, SpatialSynthSound*) override;
    void stopNote(float velocity, bool allowTailOff) override;

    void renderNextBlock(AudioBuffer<float>&, int startSample, int numSamples) override;
    using SpatialSynthVoice::renderNextBlock;

private:
    //==============================================================================
    double mPitchRatio = 0;
    double mSourceSamplePosition = 0;

    ADSR   mAdsr;

    JUCE_LEAK_DETECTOR (SpatialSamplerVoice)
};

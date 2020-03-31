#include "SpatialSampler.h"


SpatialSamplerSound::SpatialSamplerSound(const String& soundName,
                            AudioFormatReader& source,
                            int note,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : mName(soundName),
      mSourceSampleRate(source.sampleRate),
      mNoteID(note)
{
    if (mSourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        mLength = jmin ((int)source.lengthInSamples,
                       (int)(maxSampleLengthSeconds * mSourceSampleRate));

        // Make mono buffer
        mSampleData.reset(new AudioBuffer<float>(1, mLength + 4));
        mSampleData->clear();
    
        // Load all audio file channels
        std::unique_ptr<AudioBuffer<float>> tempBuffer = std::make_unique<AudioBuffer<float>>(mSampleData->getNumChannels(), mSampleData->getNumSamples());
        source.read(tempBuffer.get(), 0, mLength + 4, 0, true, true);
        
        // Sum channels to mono
        for (int i = 0; i < tempBuffer->getNumChannels(); ++i)
            mSampleData->addFrom(0, 0, *tempBuffer, i, 0, tempBuffer->getNumSamples());
        
        // Average
        mSampleData->applyGain(1.0f / (float)tempBuffer->getNumChannels());

        mAdsrParams.attack  = static_cast<float>(attackTimeSecs);
        mAdsrParams.release = static_cast<float>(releaseTimeSecs);
    }
}

SpatialSamplerSound::~SpatialSamplerSound()
{
}


//==============================================================================
SpatialSamplerVoice::SpatialSamplerVoice() {}
SpatialSamplerVoice::~SpatialSamplerVoice() {}

bool SpatialSamplerVoice::canPlaySound(SpatialSynthSound* sound)
{
    return dynamic_cast<const SpatialSamplerSound*>(sound) != nullptr;
}

void SpatialSamplerVoice::startNote(int noteID, float velocity, const glm::vec3& pos, SpatialSynthSound* s)
{
    if (auto* sound = dynamic_cast<const SpatialSamplerSound*>(s))
    {
        /*pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                        * sound->sourceSampleRate / getSampleRate();*/
        mPitchRatio = 1.0;
        mPosition = pos;
        mSourceSamplePosition = 0.0;
        mNeedsDBAPUpdate = true;
        mCurrentNoteID = noteID;

        mAdsr.setSampleRate (sound->mSourceSampleRate);
        mAdsr.setParameters (sound->mAdsrParams);

        mAdsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SpatialSamplerSounds!
    }
}

void SpatialSamplerVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        mAdsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        mAdsr.reset();
    }
}


//==============================================================================

void SpatialSamplerVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<SpatialSamplerSound*>(getCurrentlyPlayingSound().get()))
    {
        // Get sample data
        auto& data = *playingSound->mSampleData;
        const float* const monoSamples = data.getReadPointer(0);
            
        const int numChannels = outputBuffer.getNumChannels();
        const float invSamples = 1.0f / (float)numSamples;
        
        // Calculate increments to smoothly interpolate channel amplitudes
        for (int i = 0; i < mChannelAmplitudes.size(); ++i)
            mChannelAmplitudeIncrements[i] = (mChannelAmplitudeTargets[i] - mChannelAmplitudes[i]) * invSamples;
        
        int i = 0;
        
        while (--numSamples >= 0)
        {
            auto pos = (int)mSourceSamplePosition;
            auto alpha = (float)(mSourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;
            
            for (int i = 0; i < mChannelAmplitudes.size(); ++i)
                mChannelAmplitudes[i] += mChannelAmplitudeIncrements[i];

            // just using a very simple linear interpolation here..
            float s = (monoSamples[pos] * invAlpha + monoSamples[pos + 1] * alpha);

            auto envelopeValue = mAdsr.getNextSample();
            
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float* out = outputBuffer.getWritePointer(ch, startSample);
            
                out[i] += s * envelopeValue * mChannelAmplitudes[ch];
            }
        
            mSourceSamplePosition += mPitchRatio;
            i++;
            
            if (mSourceSamplePosition > playingSound->mLength)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}

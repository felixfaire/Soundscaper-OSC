/*
  ==============================================================================

    AudioFileSource.h
    Created: 29 Feb 2020 9:11:18pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/** This Audio source plays an audio file on loop and sequentially
    allocates the channels in the file to the desired number of output channels.
*/
class AudioFileSource : AudioSource
{
public:
    AudioFileSource(const String& name,
                    AudioFormatReader& source)
      : mName(name),
        mSourceSampleRate(source.sampleRate)
    {
        if (mSourceSampleRate > 0 && source.lengthInSamples > 0)
        {
            const int length = (int)source.lengthInSamples + 4;
            mData.reset(new AudioBuffer<float>((int)source.numChannels, length));

            source.read(mData.get(), 0, length, 0, true, true);
        }
    }
    

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        
    }

    void releaseResources() override
    {
    
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // Get sample data
        const auto&         data = *mData;
        AudioBuffer<float>& outputBuffer = *bufferToFill.buffer;
        const int           startSample = bufferToFill.startSample;
        int                 numSamples = bufferToFill.numSamples;
            
        const int fileChannels = data.getNumChannels();
        const int outChannels = outputBuffer.getNumChannels();
        
        // Basic amplitude smoothing
        mAmplitude += (mTargetAmplitude - mAmplitude) * 0.1f;
    
        int i = 0;
        
        while (--numSamples >= 0)
        {
            auto pos = (int)mSourceSamplePosition;
            auto alpha = (float)(mSourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;
            
            for (int ch = 0; ch < outChannels; ++ch)
            {
                const float* const inCh = data.getReadPointer(ch % fileChannels);
                const float interp = (inCh[pos] * invAlpha + inCh[pos + 1] * alpha);
                
                float* out = outputBuffer.getWritePointer(ch, startSample);
                out[i] += interp * mAmplitude;
            }
        
            mSourceSamplePosition += 1.0;
            i++;
            
            if (mSourceSamplePosition > mData->getNumSamples())
                mSourceSamplePosition -= mData->getNumSamples();
        }
    }
    
    void setAmplitude(float newAmp)
    {
        mTargetAmplitude = newAmp;
    }
    
    const AudioBuffer<float>* getAudioData() { return mData.get(); }
    
private:
    float   mAmplitude = 0.0f;
    float   mTargetAmplitude = 0.0f;
    String  mName;
    std::unique_ptr<AudioBuffer<float>> mData;
    double  mSourceSampleRate;
    double  mSourceSamplePosition = 0;
    
};

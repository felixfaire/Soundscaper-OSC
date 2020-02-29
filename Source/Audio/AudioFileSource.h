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
        auto& data = *mData;
        AudioBuffer<float>& outputBuffer = *bufferToFill.buffer;
        int startSample = bufferToFill.startSample;
        int numSamples = bufferToFill.numSamples;
            
        const int fileChannels = data.getNumChannels();
        const int outChannels = outputBuffer.getNumChannels();
    
        int i = 0;
        
        while (--numSamples >= 0)
        {
            auto pos = (int)mSourceSamplePosition;
            auto alpha = (float)(mSourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;
            
            for (int ch = 0; ch < outChannels; ++ch)
            {
                const float* const inCh = data.getReadPointer(ch % fileChannels);
                // just using a very simple linear interpolation here..
                const float interp = (inCh[pos] * invAlpha + inCh[pos + 1] * alpha);
                
                float* out = outputBuffer.getWritePointer(ch, startSample);
            
                out[i] += interp;
            }
        
            mSourceSamplePosition += 1.0;
            i++;
            
            if (mSourceSamplePosition > mData->getNumSamples())
                mSourceSamplePosition -= mData->getNumSamples();
        }
    }
    
    String  mName;
    std::unique_ptr<AudioBuffer<float>> mData;
    double  mSourceSampleRate;
    double  mSourceSamplePosition = 0;
    
};

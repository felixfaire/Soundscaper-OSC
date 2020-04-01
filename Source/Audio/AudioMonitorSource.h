/*
  ==============================================================================

    AudioMonitorSource.h
    Created: 6 Mar 2020 5:02:37pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AudioMonitorSource : AudioSource
{
public:
    AudioMonitorSource()
    {
    
    }
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        AudioBuffer<float>& outputBuffer = *bufferToFill.buffer;
        const int           startSample = bufferToFill.startSample;
        int                 numSamples = bufferToFill.numSamples;
        const int           numOutputChannels = outputBuffer.getNumChannels();
        
        ScopedLock lock(mLevelsMutex);

        if (numOutputChannels != mLevels.size())
            mLevels.resize(numOutputChannels);
        
        for (int ch = 0; ch < numOutputChannels; ++ch)
        {
            const float level = outputBuffer.getMagnitude(ch, startSample, numSamples);

            if (level > 1.0f)
            {
                for (int i = startSample; i < startSample + numSamples - 1; ++i) 
                    DBG(std::to_string(i) + ": " + std::to_string(outputBuffer.getSample(ch, i)));

                jassert(false);
            }

            mLevels[ch] = level;
        }
    }
    
    void releaseResources() override
    {
        
    }

    std::vector<float> getLevels() 
    {
        ScopedLock lock(mLevelsMutex);
        return mLevels; 
    }
    
private:

    CriticalSection    mLevelsMutex;
    std::vector<float> mLevels;
    
};

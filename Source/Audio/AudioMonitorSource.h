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
        
        if (numOutputChannels != mLevels.size())
            mLevels.resize(numOutputChannels);
        
        for (int ch = 0; ch < numOutputChannels; ++ch)
            mLevels[ch] = outputBuffer.getRMSLevel(ch, startSample, numSamples);
    }
    
    void releaseResources() override
    {
        
    }
    

    std::vector<float> mLevels;
    
};

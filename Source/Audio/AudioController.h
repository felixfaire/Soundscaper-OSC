/*
  ==============================================================================

    AudioController.h
    Created: 13 Feb 2020 9:42:56pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "SpatialSynth.h"

class AudioController   : public AudioSource
{
public:
    AudioController()
    {
    }
    
    ~AudioController()
    {
        shutdownAudio();
        jassert (mAudioSourcePlayer.getCurrentSource() == nullptr);
    }
    
    // ===== Config ===============================
    
    void setAudioChannels (int numInputChannels, int numOutputChannels, const XmlElement* const storedSettings = nullptr)
    {
        String audioError = mDeviceManager.initialise (numInputChannels, numOutputChannels, storedSettings, true);

        jassert (audioError.isEmpty());

        mDeviceManager.addAudioCallback (&mAudioSourcePlayer);
        mAudioSourcePlayer.setSource (this);
    }

    void shutdownAudio()
    {
        mAudioSourcePlayer.setSource (nullptr);
        mDeviceManager.removeAudioCallback (&mAudioSourcePlayer);
    }
    
    // ===== Processing ===============================
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        String message;
        message << "Preparing to play audio..." << newLine;
        message << " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine;
        message << " sampleRate = " << sampleRate;
        Logger::getCurrentLogger()->writeToLog (message);
        
        mSynth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        mSynth.renderNextBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
    }

    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }
    
    SpatialSynth         mSynth;
    
    AudioDeviceManager& getDeviceManager() { return mDeviceManager; }
    
private:

    AudioDeviceManager mDeviceManager;
    AudioSourcePlayer  mAudioSourcePlayer;
    
};

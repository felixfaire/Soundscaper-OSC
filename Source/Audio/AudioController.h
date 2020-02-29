/*
  ==============================================================================

    AudioController.h
    Created: 13 Feb 2020 9:42:56pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "SpatialSynth.h"
#include "SpatialSampler.h"
#include "SoundEventData.h"

class AudioController   : public AudioSource
{
public:
    AudioController()
    {
        mFormatManager.registerBasicFormats();
        
        mSoundEventData.onProcessEvent = [&](const SoundEvent& e) {
            if (e.isStartNote())
                mSynth.noteOn(e.noteID, e.soundID, 1.0f, e.position);
            else
                mSynth.handlePositionChange(e.noteID, e.position);
        };
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
        
        mSynth.setSampleRate(sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        mSoundEventData.processEventData();
        mSynth.renderNextBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
    }
    
    void loadAudioFiles(AppModel& model)
    {
        mSynth.clearSounds();
        mSynth.clearVoices();
        
        File folder = model.mCurrentAudioFolder;
        model.mSoundFiles = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
        
        int noteNum = 0;
        
        for (auto& wavFile : model.mSoundFiles)
        {
            std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(wavFile));
            
            if (reader != nullptr)
            {
                Logger::getCurrentLogger()->writeToLog("Loading file: " + wavFile.getFileNameWithoutExtension());
                auto* newSound = new SpatialSamplerSound(wavFile.getFileNameWithoutExtension(), *reader, noteNum, 0.01, 0.5, 20.0);
                mSynth.addSound(newSound);
                mSynth.addVoice(new SpatialSamplerVoice());
                noteNum++;
            }
        }
        
        if (model.mSoundFiles.size() == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Failed to find any .wavs");
        }
    }

    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }
    
    // Manages lockfree message processing with a fifo
    void addSoundEvent(const SoundEvent& event)
    {
        mSoundEventData.addSoundEvent(event);
    }
        
    AudioDeviceManager& getDeviceManager() { return mDeviceManager; }

    // All public access to this method on the message thread
    // will cause locking, so only use on initialisation etc.
    SpatialSynth       mSynth;

private:

    SoundEventData     mSoundEventData;

    AudioDeviceManager mDeviceManager;
    AudioSourcePlayer  mAudioSourcePlayer;
    
    // File loading
    AudioFormatManager  mFormatManager;
    
};

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
#include "AudioFileSource.h"
#include "AudioMonitorSource.h"

class AudioController   : public AudioSource
{
public:
    AudioController(AudioDeviceManager& deviceManager)
        : mDeviceManager(deviceManager)
    {
        mFormatManager.registerBasicFormats();
        
        mSoundEventData.onProcessEvent = [&](const SoundEvent& e) {
            if (e.isStartNote())
                mSynth.noteOn(e.noteID, e.soundID, 1.0f, e.position);
            else
                mSynth.handlePositionChange(e.noteID, e.position);
        };
        
        mMonitor.reset(new AudioMonitorSource());
    }
    
    ~AudioController()
    {
        shutdownAudio();
        jassert (mAudioSourcePlayer.getCurrentSource() == nullptr);
    }
    
    // ===== Config ===============================
    
    void initialise()
    {
        mDeviceManager.addAudioCallback(&mAudioSourcePlayer);
        mAudioSourcePlayer.setSource(this);
    }

    void shutdownAudio()
    {
        mAudioSourcePlayer.setSource(nullptr);
        mDeviceManager.removeAudioCallback(&mAudioSourcePlayer);
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
        
        for (auto& src : mAtmosphereSources)
            src->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        mSoundEventData.processEventData();
        mSynth.renderNextBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
        
        for (auto& atmosphere : mAtmosphereSources)
            atmosphere->getNextAudioBlock(bufferToFill);
            
        mMonitor->getNextAudioBlock(bufferToFill);
    }
    
    void releaseResources() override
    {
        Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
    }
    
    
    // ===== Events ===========================
    
    void loadAudioFiles(AudioDataState& data)
    {
        // Load soundatmosphere files
        auto atmosphereFiles = data.mCurrentSoundAtmosphereFolder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
        data.mSoundAtmosphereData.clear();
        mAtmosphereSources.clear();
        
        for (auto& atmosphereFile : atmosphereFiles)
        {
            std::unique_ptr<AudioFormatReader> atmosphereReader(mFormatManager.createReaderFor(atmosphereFile));
            
            if (atmosphereReader != nullptr)
            {
                const auto name = atmosphereFile.getFileNameWithoutExtension();
                auto* newSound = new AudioFileSource(name, *atmosphereReader);
                const double fileLength = (double)atmosphereReader->lengthInSamples / (double)atmosphereReader->sampleRate;
                data.addSoundAtmosphereData(name, newSound->getAudioData(), fileLength);
                mAtmosphereSources.emplace_back(newSound);
            }
        }
        
        // Load spatial clip files
        data.mSoundClipData.clear();
        
        mSynth.clearSounds();
        mSynth.clearVoices();
        
        File folder = data.mCurrentSoundClipFolder;
        auto clipFiles = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
        
        int noteID = 0;
        
        for (auto& wavFile : clipFiles)
        {
            std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(wavFile));
            
            if (reader != nullptr)
            {
                const auto name = wavFile.getFileNameWithoutExtension();
                auto* newSound = new SpatialSamplerSound(wavFile.getFileNameWithoutExtension(), *reader, noteID, 0.01, 0.5, 20.0);
                const double fileLength = (double)reader->lengthInSamples / (double)reader->sampleRate;

                data.addSoundClipData(newSound->getName(), newSound->getAudioData(), fileLength);
                
                mSynth.addSound(newSound);
                mSynth.addVoice(new SpatialSamplerVoice());
                noteID++;
            }
        }
        
        if (clipFiles.size() == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Failed to find any .wavs");
        }

        data.sendChangeMessage();
    }
    
    // Manages lockfree message processing with a fifo
    void addSoundEvent(const SoundEvent& event)
    {
        mSoundEventData.addSoundEvent(event);
    }
    
    void setSoundAtmosphereAmplitudes(const std::vector<float>& amps)
    {
        // TODO: NOT THREAD SAFE YET
        jassert(amps.size() == mAtmosphereSources.size());
        
        for (int i = 0; i < amps.size(); ++i)
            mAtmosphereSources[i]->setAmplitude(amps[i]);
    }
    
    std::vector<float>& getAudioLevels()
    {
        // TODO: NOT THREAD SAFE YET
        return mMonitor->mLevels;
    }
        
    AudioDeviceManager& getDeviceManager() { return mDeviceManager; }

    // All public access to this method on the message thread
    // will cause locking, so only use on initialisation etc.
    SpatialSynth       mSynth;

private:

    std::vector<std::unique_ptr<AudioFileSource>>    mAtmosphereSources;
    std::unique_ptr<AudioMonitorSource> mMonitor;

    SoundEventData     mSoundEventData;

    AudioDeviceManager& mDeviceManager;
    AudioSourcePlayer   mAudioSourcePlayer;
    
    // File loading
    AudioFormatManager  mFormatManager;
    
};

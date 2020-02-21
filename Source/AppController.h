/*
  ==============================================================================

    AppController.h
    Created: 13 Feb 2020 9:04:00pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "AppModel.h"
#include "Audio/AudioController.h"
#include "Audio/SpatialSampler.h"

/** This class contains the core functionality of the app.
    handling the passing of messages to trigger sounds and
    manage control data.
*/
class AppController
{
public:
    AppController(AppModel& model)
        : mModel(model)
    {
        mFormatManager.registerBasicFormats();
        mAudio.setAudioChannels(0, 2);
        mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositions);
    }
    
    void loadAudioFiles()
    {
        mAudio.mSynth.clearSounds();
        mAudio.mSynth.clearVoices();
        
        File folder = mModel.mCurrentAudioFolder;
        mModel.mSoundFiles = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
        
        int noteNum = 0;
        
        for (auto& wavFile : mModel.mSoundFiles)
        {
            std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(wavFile));
            
            if (reader != nullptr)
            {
                Logger::getCurrentLogger()->writeToLog("Loading file: " + wavFile.getFileNameWithoutExtension());
                auto* newSound = new SpatialSamplerSound(wavFile.getFileNameWithoutExtension(), *reader, noteNum, 0.01, 1.0, 5.0);
                mAudio.mSynth.addSound(newSound);
                mAudio.mSynth.addVoice(new SpatialSamplerVoice());
                noteNum++;
            }
        }
        
        if (mModel.mSoundFiles.size() == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Failed to find any .wavs");
        }
    }
    
    void triggerSource(int soundID, const glm::vec3& pos)
    {
        const int noteID = ++mModel.mCurrentNoteID;
        mAudio.mSynth.noteOn(noteID, soundID, 1.0f, pos);
    }
    
    void updateSource(int soundID, const glm::vec3& pos)
    {
        const int noteID = mModel.mCurrentNoteID;
        mAudio.mSynth.handlePositionChange(noteID, pos);
    }
    
    AudioDeviceManager& getDeviceManager() { return mAudio.getDeviceManager(); }
    
private:

    AppModel&           mModel;
    AudioController     mAudio;
    AudioFormatManager  mFormatManager;
    
};

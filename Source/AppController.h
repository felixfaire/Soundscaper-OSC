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
    }
    
    void loadAudioFiles()
    {
        mAudio.mSynth.clearSounds();
        mAudio.mSynth.clearVoices();
        
        File folder = mModel.mCurrentAudioFolder;
        mModel.mSoundFiles = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
        
        int noteNum = 60;
        
        for (auto& wavFile : mModel.mSoundFiles)
        {
            std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(wavFile));
            
            if (reader != nullptr)
            {
                Logger::getCurrentLogger()->writeToLog("Loading file: " + wavFile.getFileNameWithoutExtension());
                auto notes = BigInteger();
                notes.setBit(noteNum, true);
                auto* newSound = new SamplerSound(wavFile.getFileNameWithoutExtension(), *reader, notes, noteNum, 0.01, 10.0, 15.0);
                mAudio.mSynth.addSound(newSound);
                mAudio.mSynth.addVoice(new SamplerVoice());
                noteNum++;
            }
        }
        
        if (mModel.mSoundFiles.size() == 0)
        {
            Logger::getCurrentLogger()->writeToLog("Failed to find any .wavs");
        }
    }
    
    void triggerSource(int index) //vec3 pos)
    {
        const int noteNum = 60 + index;
        mAudio.mSynth.noteOn(1, noteNum, 1.0f);
    }
    
    AudioDeviceManager& getDeviceManager() { return mAudio.getDeviceManager(); }
    
private:

    AppModel&           mModel;
    AudioController     mAudio;
    AudioFormatManager  mFormatManager;
    
};

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
class AppController         : public ChangeListener,
                              private OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
    AppController(AppModel& model)
        : mModel(model)
    {
        mFormatManager.registerBasicFormats();
        mAudio.setAudioChannels(0, 2);
        mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositions);

        mModel.addChangeListener(this);
        mModel.mOSCReciever.addListener(this);
    }
    
    ~AppController()
    {
        mModel.addChangeListener(this);
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
                auto* newSound = new SpatialSamplerSound(wavFile.getFileNameWithoutExtension(), *reader, noteNum, 0.01, 0.5, 20.0);
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
        jassert(soundID < mModel.mSoundFiles.size());
        const int noteID = ++mModel.mCurrentNoteID;
        mAudio.addSoundEvent({noteID, soundID, pos});
    }
    
    void updateSource(int soundID, const glm::vec3& pos)
    {
        jassert(soundID < mModel.mSoundFiles.size());
        const int noteID = mModel.mCurrentNoteID;
        mAudio.addSoundEvent({noteID, -1, pos});
    }
    
    void allNotesOff()
    {
        mAudio.mSynth.allNotesOff(true);
    }
    
    AudioDeviceManager& getDeviceManager() { return mAudio.getDeviceManager(); }
    
private:

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        jassert(dynamic_cast<AppModel*>(source) != nullptr);
        
        mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositions);
    }

    virtual void oscMessageReceived(const OSCMessage& message) override
    {
        // TODO: convert this to not use strings
        if (message.getAddressPattern().toString() == "/sound/position")
        {
            if (message[0].isInt32())
            {
                const int id = message[0].getInt32();
                triggerSource(id, glm::vec3(0.0f));
            }
            else
            {
                DBG("Incorrect message type");
            }
        }
    }

    AppModel&           mModel;
    AudioController     mAudio;

    // File loading
    AudioFormatManager  mFormatManager;
    
};

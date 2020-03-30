/*
  ==============================================================================

    AppModel.h
    Created: 12 Feb 2020 10:57:57pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "../Audio/SpatialSampler.h"

#include "SpeakerPositionsState.h"
#include "AudioMonitorState.h"
#include "AudioDataState.h"
#include "AtmosphereLevelState.h"


struct VoiceData
{
    double mStartTime;
    String mFileName;
    int    mID;
};

struct AppModel
{
public:
    AppModel()
    {
    }

    ~AppModel()
    {
    }
           
    SpeakerPositionsState               mSpeakerPositionsState;
    AudioMonitorState                   mAudioMonitorState;
    AudioDataState                      mAudioDataState;
    AtmosphereLevelState                mAtmosphereLevelState;

    std::vector<VoiceData>              mPlayingVoiceData; // TODO: unused
    int                                 mCurrentNoteID = 0; // TODO: make this clear its mouse specific
    
    // IO Devices
    OSCReceiver                         mOSCReciever;
    AudioDeviceManager                  mDeviceManager;

private:

    std::unique_ptr<PropertiesFile>     mSettingsFile;

    friend class AppModelLoader;
    
};

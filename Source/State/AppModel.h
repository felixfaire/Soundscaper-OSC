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
#include "VisualPlayingVoicesState.h"


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
    VisualPlayingVoicesState            mVisualVoiceState;
    
    // Used by the mouse interface
    int                                 mCurrentMouseNoteID = -99999;
    
    // IO Devices
    OSCReceiver                         mOSCReciever;
    AudioDeviceManager                  mDeviceManager;

private:

    std::unique_ptr<PropertiesFile>     mSettingsFile;

    friend class AppModelLoader;
    
};

/*
  ==============================================================================

    AppModeSaveLoad.h
    Created: 2 Mar 2020 4:39:31pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "../AppModel.h"

class AppModelLoader
{
public:
    static void loadSettings(AppModel& m);
    static void saveSettings(AppModel& m);
    
    // Settings ID's
    static const String   mCurrentSoundClipFolderID;
    static const String   mCurrentSoundBedFolderID;
    static const String   mSpeakerInfoID;
    static const String   mAudioDeviceInfoID;

};

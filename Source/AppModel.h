/*
  ==============================================================================

    AppModel.h
    Created: 12 Feb 2020 10:57:57pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once
#include "vec3.hpp"

struct SpeakerInfo
{
    glm::vec3 position;
};

struct AppModel
{
public:
    AppModel()
    {
        mCurrentAudioFolder = File::getCurrentWorkingDirectory().getChildFile("../../../../assets/samples");
        
        // Load default stereo model
        mSpeakers.add({glm::vec3(-0.5f, 0.0f, 0.0f)});
        mSpeakers.add({glm::vec3(0.5f, 0.0f, 0.0f)});
    }
        
    Array<File>         mSoundFiles;
    Array<String>       mOSCAddresses;
    Array<SpeakerInfo>  mSpeakers;
    
    File                mCurrentAudioFolder;
    
};

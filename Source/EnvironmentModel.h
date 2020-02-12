/*
  ==============================================================================

    EnvironmentModel.h
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

struct EnvironmentModel
{
public:
    EnvironmentModel()
    {}
    
    Array<File>         mSoundFiles;
    Array<String>       mOSCAddresses;
    Array<SpeakerInfo>  mSpeakers;
};

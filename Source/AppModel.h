/*
  ==============================================================================

    AppModel.h
    Created: 12 Feb 2020 10:57:57pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once
#include "vec3.hpp"


struct AppModel
{
public:
    AppModel()
    {
        mCurrentAudioFolder = File::getCurrentWorkingDirectory();
        File parentSearch = mCurrentAudioFolder;

        for (int i = 0; i < 5; ++i)
        {
            parentSearch = parentSearch.getParentDirectory();
            
            if (parentSearch.getFileNameWithoutExtension().toLowerCase() == "oscaudioplayer")
            {
                mCurrentAudioFolder = parentSearch.getChildFile("assets/samples");
                break;
            }
        }
        
        // Load default stereo model
        const float r = 1.5f;
        mSpeakerPositions.push_back(glm::vec3(-r, 0.0f, 0.0f));
        mSpeakerPositions.push_back(glm::vec3( r, 0.0f, 0.0f));
        mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f, r));
        mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f,-r));
    }
        
    Array<File>               mSoundFiles;
    std::vector<String>       mOSCAddresses;
    std::vector<glm::vec3>    mSpeakerPositions;
    
    File                mCurrentAudioFolder;
    
    int                 mCurrentNoteID = 0;
    
};

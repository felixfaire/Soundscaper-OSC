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
        // Setup properties file
        auto settingsOpts = PropertiesFile::Options();
        settingsOpts.filenameSuffix = ".settings";
        settingsOpts.applicationName = "SoundscaperOSC";
        settingsOpts.folderName = "Synaesthete";
        settingsOpts.osxLibrarySubFolder = "Application Support";
        settingsOpts.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
        mSettingsFile.reset(new PropertiesFile(settingsOpts));

        mSettingsFile->reload();

        if (mSettingsFile->containsKey("audio-files-location"))
            mCurrentAudioFolder = mSettingsFile->getValue("audio-files-location");
        
        // Load default stereo model
        const float r = 1.0f;
        mSpeakerPositions.push_back(glm::vec3(-r, 0.0f, 0.0f));
        mSpeakerPositions.push_back(glm::vec3( r, 0.0f, 0.0f));
        mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f, r));
        mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f,-r));
    }

    ~AppModel()
    {
        mSettingsFile->setValue("audio-files-location", mCurrentAudioFolder.getFullPathName());
        mSettingsFile->save();
    }

    std::unique_ptr<PropertiesFile>     mSettingsFile;
        
    Array<File>                         mSoundFiles;
    std::vector<glm::vec3>              mSpeakerPositions;

    std::vector<OSCAddressPattern>      mOSCAddresses;
    OSCReceiver                         mOSCReciever;
    
    File                                mCurrentAudioFolder;
    
    int                                 mCurrentNoteID = 0;
    
};

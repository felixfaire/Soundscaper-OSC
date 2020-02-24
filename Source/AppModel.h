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
        loadSettings();
    }

    ~AppModel()
    {
        saveSettings();
    }

    std::unique_ptr<PropertiesFile>     mSettingsFile;
        
    Array<File>                         mSoundFiles;
    std::vector<glm::vec3>              mSpeakerPositions;

    std::vector<OSCAddressPattern>      mOSCAddresses;
    OSCReceiver                         mOSCReciever;
    
    File                                mCurrentAudioFolder;
    
    int                                 mCurrentNoteID = 0;

private:


    void loadSettings()
    {
        // Setup and load properties file
        auto settingsOpts = PropertiesFile::Options();
        settingsOpts.filenameSuffix = ".settings";
        settingsOpts.applicationName = "SoundscaperOSC";
        settingsOpts.folderName = "Synaesthete";
        settingsOpts.osxLibrarySubFolder = "Application Support";
        settingsOpts.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
        settingsOpts.commonToAllUsers = false;
        mSettingsFile.reset(new PropertiesFile(settingsOpts));

        mSettingsFile->reload();

        if (mSettingsFile->containsKey(mCurrentAudioFolderID))
            mCurrentAudioFolder = mSettingsFile->getValue(mCurrentAudioFolderID);

        if (mSettingsFile->containsKey(mSpeakerInfoID))
        {
            auto speakersInfo = mSettingsFile->getXmlValue(mSpeakerInfoID);

            forEachXmlChildElement(*speakersInfo, s)
            {
                glm::vec3 pos;
                pos.x = (float)s->getDoubleAttribute("x", 0.0);
                pos.y = (float)s->getDoubleAttribute("y", 0.0);
                pos.z = (float)s->getDoubleAttribute("z", 0.0);

                mSpeakerPositions.push_back(pos);
            }
        }
        else
        {
            // Load default stereo model
            const float r = 1.0f;
            mSpeakerPositions.push_back(glm::vec3(-r, 0.0f, 0.0f));
            mSpeakerPositions.push_back(glm::vec3( r, 0.0f, 0.0f));
            mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f, r));
            mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f,-r));
        }
    }

    void saveSettings()
    {
        mSettingsFile->setValue(mCurrentAudioFolderID, mCurrentAudioFolder.getFullPathName());

        XmlElement speakersProps(mSpeakerInfoID);

        for (const auto& s : mSpeakerPositions)
        {
            XmlElement* speaker = new XmlElement("speaker");
            speaker->setAttribute("x", s.x);
            speaker->setAttribute("y", s.y);
            speaker->setAttribute("z", s.z);

            speakersProps.addChildElement(speaker);
        }

        mSettingsFile->setValue(mSpeakerInfoID, &speakersProps);
        mSettingsFile->save();
    }

    // Settings ID's
    String                              mCurrentAudioFolderID = "audio-files-location";
    String                              mSpeakerInfoID = "speaker-info";
    
};

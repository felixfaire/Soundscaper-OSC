/*
  ==============================================================================

    AppModelLoader.cpp
    Created: 2 Mar 2020 4:56:55pm
    Author:  Felix Faire

  ==============================================================================
*/

#include "AppModelLoader.h"

// Settings ID's
const String AppModelLoader::mCurrentSoundClipFolderID = "audio-clip-files-location";
const String AppModelLoader::mCurrentSoundAtmosphereFolderID = "audio-atmosphere-files-location";
const String AppModelLoader::mSpeakerInfoID = "speaker-info";
const String AppModelLoader::mAudioDeviceInfoID = "audio-device-info";

void AppModelLoader::loadSettings(AppModel& m)
{
    // Setup and load properties file
    auto settingsOpts = PropertiesFile::Options();
    settingsOpts.filenameSuffix = ".settings";
    settingsOpts.applicationName = "SoundscaperOSC";
    settingsOpts.folderName = "Synaesthete";
    settingsOpts.osxLibrarySubFolder = "Application Support";
    settingsOpts.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
    settingsOpts.commonToAllUsers = false;
    m.mSettingsFile.reset(new PropertiesFile(settingsOpts));

    m.mSettingsFile->reload();
    
    // Init audio device settings
    
    if (m.mSettingsFile->containsKey(mAudioDeviceInfoID))
    {
        auto deviceSettings = m.mSettingsFile->getXmlValue(mAudioDeviceInfoID);
        String audioError = m.mDeviceManager.initialise(0, 2, deviceSettings.get(), true);
        jassert(audioError.isEmpty());
    }
    else
    {
        m.mDeviceManager.initialise(0, 2, nullptr, true);
    }
    
    // Init app settings

    if (m.mSettingsFile->containsKey(mCurrentSoundClipFolderID))
        m.mCurrentSoundClipFolder = m.mSettingsFile->getValue(mCurrentSoundClipFolderID);
        
    if (m.mSettingsFile->containsKey(mCurrentSoundAtmosphereFolderID))
        m.mCurrentSoundAtmosphereFolder = m.mSettingsFile->getValue(mCurrentSoundAtmosphereFolderID);

    if (m.mSettingsFile->containsKey(mSpeakerInfoID))
    {
        auto speakersInfo = m.mSettingsFile->getXmlValue(mSpeakerInfoID);

        forEachXmlChildElement(*speakersInfo, s)
        {
            glm::vec3 pos;
            pos.x = (float)s->getDoubleAttribute("x", 0.0);
            pos.y = (float)s->getDoubleAttribute("y", 0.0);
            pos.z = (float)s->getDoubleAttribute("z", 0.0);

            m.mSpeakerPositionsState.mSpeakerPositions.push_back(pos);
        }
    }
    else
    {
        // Load default stereo model
        const float r = 2.0f;
        m.mSpeakerPositionsState.mSpeakerPositions.push_back(glm::vec3(-r, 0.0f, 0.0f));
        m.mSpeakerPositionsState.mSpeakerPositions.push_back(glm::vec3( r, 0.0f, 0.0f));
        m.mSpeakerPositionsState.mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f, -r));
        m.mSpeakerPositionsState.mSpeakerPositions.push_back(glm::vec3(0.0f, 0.0f,  r));
    }
    
    m.mSpeakerPositionsState.sendChangeMessage();
}

void AppModelLoader::saveSettings(AppModel& m)
{
    m.mSettingsFile->setValue(mCurrentSoundClipFolderID, m.mCurrentSoundClipFolder.getFullPathName());
    m.mSettingsFile->setValue(mCurrentSoundAtmosphereFolderID, m.mCurrentSoundAtmosphereFolder.getFullPathName());

    XmlElement speakersProps(mSpeakerInfoID);

    for (const auto& s : m.mSpeakerPositionsState.getPositions())
    {
        XmlElement* speaker = new XmlElement("speaker");
        speaker->setAttribute("x", s.x);
        speaker->setAttribute("y", s.y);
        speaker->setAttribute("z", s.z);

        speakersProps.addChildElement(speaker);
    }
    
    auto audioDeviceProps = m.mDeviceManager.createStateXml();

    if (audioDeviceProps != nullptr)
        m.mSettingsFile->setValue(mAudioDeviceInfoID, audioDeviceProps.get());
        
    m.mSettingsFile->setValue(mSpeakerInfoID, &speakersProps);
    m.mSettingsFile->save();
}

/*
  ==============================================================================

    AppModel.h
    Created: 12 Feb 2020 10:57:57pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once
#include "vec3.hpp"
#include "Audio/SpatialSampler.h"

struct SoundFileData
{
    SoundFileData(const String& name, const AudioBuffer<float>* data)
        : mName(name),
          mAudioData(data)
    {
        // TODO: make a proper address
        mOSCAddress = mName.replace(" ", "_") + "/";
    }
    
    String                              mName;
    String                              mOSCAddress;
    const AudioBuffer<float>*           mAudioData;
};

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
    
    void addSpeaker(const glm::vec3& pos)
    {
        mSpeakerPositions.push_back(pos);
        mSpeakerPositionsChanges.sendChangeMessage();
    }
    
    void removeSpeaker()
    {
        mSpeakerPositions.pop_back();
        mSpeakerPositionsChanges.sendChangeMessage();
    }
    
    void setSpeakerPosition(int index, const glm::vec3& pos)
    {
        jassert(index < mSpeakerPositions.size());
        mSpeakerPositions[index] = pos;
        mSpeakerPositionsChanges.sendChangeMessage();
    }
    
    const std::vector<glm::vec3>& getSpeakerPositions() const { return mSpeakerPositions; }
    const glm::vec3& getSpeakerPosition(int i) const          { return mSpeakerPositions[i]; }
    
    void setSoundbedAmplitude(int index, float newAmplitude)
    {
        jassert(mSoundBedAmplitudes.size() == mSoundBedData.size());
        jassert(index < mSoundBedAmplitudes.size());
        mSoundBedAmplitudes[index] = newAmplitude;
        mSoundBedAmplitudesChanges.sendChangeMessage();
    }
    
    const std::vector<float>& getSoundBedAmpitudes() const { return mSoundBedAmplitudes; }
    float getSoundBedAmpitude(int i) const                 { return mSoundBedAmplitudes[i]; }
    
    
    
    
    std::unique_ptr<PropertiesFile>     mSettingsFile;
        
        
    ChangeBroadcaster                   mSoundBedAmplitudesChanges;
    ChangeBroadcaster                   mSpeakerPositionsChanges;

    
    File                                mCurrentSoundBedFolder;
    std::vector<SoundFileData>          mSoundBedData;
    
    File                                mCurrentSoundClipFolder;
    std::vector<SoundFileData>          mSoundClipData;
    
    std::vector<VoiceData>              mPlayingVoiceData;
    
    int                                 mCurrentNoteID = 0;
    
    
    // IO Devices
    OSCReceiver                         mOSCReciever;
    AudioDeviceManager                  mDeviceManager;

private:

    // These must be changed via get / set to ensure all change messages are propogated correctly
    std::vector<float>                  mSoundBedAmplitudes;
    std::vector<glm::vec3>              mSpeakerPositions;
    
    friend class AppModelLoader;
    
};

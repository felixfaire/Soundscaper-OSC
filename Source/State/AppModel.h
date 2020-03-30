/*
  ==============================================================================

    AppModel.h
    Created: 12 Feb 2020 10:57:57pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once
#include "vec3.hpp"
#include "../Audio/SpatialSampler.h"
#include "SpeakerPositionsState.h"

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
    
    void setSoundAtmosphereAmplitude(int index, float newAmplitude)
    {
        jassert(mSoundAtmosphereAmplitudes.size() == mSoundAtmosphereData.size());
        jassert(index < mSoundAtmosphereAmplitudes.size());
        
        if (index >= mSoundAtmosphereAmplitudes.size())
            return;
        
        mSoundAtmosphereAmplitudes[index] = newAmplitude;
        mSoundAtmosphereAmplitudesChanges.sendChangeMessage();
    }
    
    const std::vector<float>&   getSoundAtmosphereAmpitudes() const { return mSoundAtmosphereAmplitudes; }
    float                       getSoundAtmosphereAmpitude(int i) const                 { return mSoundAtmosphereAmplitudes[i]; }
    
    void addSoundAtmosphereData(const String& name, const AudioBuffer<float>* data)
    {
        mSoundAtmosphereData.emplace_back(name, data);
        mSoundAtmosphereAmplitudes.push_back(1.0f);
    }
    
    void addSoundClipData(const String& name, const AudioBuffer<float>* data)
    {
        mSoundClipData.emplace_back(name, data);
    }
    
    void setAudioLevels(const std::vector<float>& newLevels)
    {
        if (newLevels.size() != mAudioLevels.size())
            mAudioLevels.resize(newLevels.size());
            
        for (int i = 0; i < newLevels.size(); ++i)
        {
            mAudioLevels[i] *= 0.9f;
            
            if (newLevels[i] > mAudioLevels[i])
                mAudioLevels[i] = newLevels[i];
        }
        
        mAudioLevelChanges.sendChangeMessage();
    }
    
    
    std::unique_ptr<PropertiesFile>     mSettingsFile;
        
        
    ChangeBroadcaster                   mSoundAtmosphereAmplitudesChanges;
    ChangeBroadcaster                   mAudioLevelChanges;

    SpeakerPositionsState                mSpeakerPositionsState;
    
    File                                mCurrentSoundAtmosphereFolder;
    std::vector<SoundFileData>          mSoundAtmosphereData;
    
    File                                mCurrentSoundClipFolder;
    std::vector<SoundFileData>          mSoundClipData;
    
    std::vector<VoiceData>              mPlayingVoiceData;
    
    int                                 mCurrentNoteID = 0;
    
    std::vector<float>                  mAudioLevels;
    
    // IO Devices
    OSCReceiver                         mOSCReciever;
    AudioDeviceManager                  mDeviceManager;

private:

    // These must be changed via get / set to ensure all change messages are propogated correctly
    std::vector<float>                  mSoundAtmosphereAmplitudes;
    
    friend class AppModelLoader;
    
};

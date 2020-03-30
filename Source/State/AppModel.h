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
        jassert(mSoundAtmosphereAmplitudes.size() == mAudioDataState.mSoundAtmosphereData.size());
        jassert(index < mSoundAtmosphereAmplitudes.size());
        
        if (index >= mSoundAtmosphereAmplitudes.size())
            return;
        
        mSoundAtmosphereAmplitudes[index] = newAmplitude;
        mSoundAtmosphereAmplitudesChanges.sendChangeMessage();
    }
    
    const std::vector<float>&   getSoundAtmosphereAmpitudes() const { return mSoundAtmosphereAmplitudes; }
    float                       getSoundAtmosphereAmpitude(int i) const                 { return mSoundAtmosphereAmplitudes[i]; }

    
   
    std::unique_ptr<PropertiesFile>     mSettingsFile;
        
    ChangeBroadcaster                   mSoundAtmosphereAmplitudesChanges;

    SpeakerPositionsState               mSpeakerPositionsState;
    AudioMonitorState                   mAudioMonitorState;
    AudioDataState                      mAudioDataState;
    

    std::vector<VoiceData>              mPlayingVoiceData; // TODO: unused
    int                                 mCurrentNoteID = 0; // TODO: make this clear its mouse specific
    
    
    // IO Devices
    OSCReceiver                         mOSCReciever;
    AudioDeviceManager                  mDeviceManager;

private:

    // These must be changed via get / set to ensure all change messages are propogated correctly
    std::vector<float>                  mSoundAtmosphereAmplitudes;
    
};

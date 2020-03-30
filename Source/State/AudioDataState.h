/*
  ==============================================================================

    AudioDataState.h
    Created: 30 Mar 2020 3:15:31pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>

struct SoundFileData
{
    SoundFileData(const String& name, const AudioBuffer<float>* data)
        : mName(name),
        mAudioData(data)
    {
        // TODO: make a proper OSC address
        mOSCAddress = mName.replace(" ", "_") + "/";
    }

    String                              mName;
    String                              mOSCAddress;
    const AudioBuffer<float>*           mAudioData;
};


class AudioDataState : public ChangeBroadcaster
{
public:
    AudioDataState()
    {

    }

    void addSoundAtmosphereData(const String& name, const AudioBuffer<float>* data)
    {
        mSoundAtmosphereData.emplace_back(name, data);
        //mSoundAtmosphereAmplitudes.push_back(1.0f);
    }

    void addSoundClipData(const String& name, const AudioBuffer<float>* data)
    {
        mSoundClipData.emplace_back(name, data);
    }

    File                                mCurrentSoundAtmosphereFolder;
    std::vector<SoundFileData>          mSoundAtmosphereData;

    File                                mCurrentSoundClipFolder;
    std::vector<SoundFileData>          mSoundClipData;

};
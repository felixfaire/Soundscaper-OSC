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
    SoundFileData(const String& name, const AudioBuffer<float>* data, double fileLength)
        : mName(name),
          mAudioData(data),
          mFileLength(fileLength)
    {
        // TODO: make a proper OSC address
        mOSCAddress = mName.replace(" ", "_") + "/";
    }

    String                              mName;
    String                              mOSCAddress;
    const AudioBuffer<float>*           mAudioData;
    double                              mFileLength;
};


class AudioDataState : public ChangeBroadcaster
{
public:
    AudioDataState()
    {

    }

    void addSoundAtmosphereData(const String& name, const AudioBuffer<float>* data, double fileLength)
    {
        mSoundAtmosphereData.emplace_back(name, data, fileLength);
    }

    void addSoundClipData(const String& name, const AudioBuffer<float>* data, double fileLength)
    {
        mSoundClipData.emplace_back(name, data, fileLength);
    }

    File                                mCurrentSoundAtmosphereFolder;
    std::vector<SoundFileData>          mSoundAtmosphereData;

    File                                mCurrentSoundClipFolder;
    std::vector<SoundFileData>          mSoundClipData;

};
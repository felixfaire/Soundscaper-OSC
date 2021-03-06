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
    SoundFileData(const String& name, const AudioBuffer<float>& data, double fileLength, int index)
        : mName(name),
          mFileLength(fileLength),
          mIndex(index)
    {
        mOSCAddress = mName.replace(" ", "_");

        generateWaveform(data);
    }

    void generateWaveform(const AudioBuffer<float>& data)
    {
        const int numSamples = data.getNumSamples();
        const int step = numSamples / mWaveformSize;
        const int windowSize = step * 2;
        const int numChannels = data.getNumChannels();

        float maxLevel = 0.01f;

        for (int i = 0; i < mWaveformSize; ++i)
        {
            const int startSample = i * step;
            const int numWindowSamples = jmin(windowSize, numSamples - startSample - 1);
            const float level = data.getRMSLevel(numChannels - 1, startSample, numWindowSamples);
            mWaveform.push_back(level);

            if (level > maxLevel)
                maxLevel = level;
        }

        for (auto& l : mWaveform)
            l = jmin(l / maxLevel, 1.0f);
    }

    String                              mName;
    String                              mOSCAddress;
    double                              mFileLength;
    int                                 mIndex;

    std::vector<float>                  mWaveform;
    int                                 mWaveformSize = 256;
};


class AudioDataState : public ChangeBroadcaster
{
public:
    AudioDataState()
    {

    }

    void addSoundAtmosphereData(const String& name, const AudioBuffer<float>& data, double fileLength)
    {
        mSoundAtmosphereData.emplace_back(name, data, fileLength, (int)mSoundAtmosphereData.size());
    }

    void addSoundClipData(const String& name, const AudioBuffer<float>& data, double fileLength)
    {
        mSoundClipData.emplace_back(name, data, fileLength, (int)mSoundClipData.size());
    }
    
    
    int getAtmosphereIndexFromClipAddress(const String& address)
    {
        for (int i = 0; i < mSoundClipData.size(); ++i)
            if (mSoundClipData[i].mOSCAddress == address)
                return i;
                
        return -1;
    }
    
    int getSoundIndexFromClipAddress(const String& address)
    {
        for (int i = 0; i < mSoundClipData.size(); ++i)
            if (mSoundClipData[i].mOSCAddress == address)
                return i;
                
        return -1;
    }

    File                                mCurrentSoundAtmosphereFolder;
    std::vector<SoundFileData>          mSoundAtmosphereData;

    File                                mCurrentSoundClipFolder;
    std::vector<SoundFileData>          mSoundClipData;

};

/*
  ==============================================================================

    VisualPlayingVoicesState.h
    Created: 30 Mar 2020 4:55:43pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include "vec3.hpp"
#include "AudioDataState.h"

struct VisualVoiceData
{
    uint32      mStartTime;
    uint32      mFileLength;
    String      mFileName;
    int         mID;
    glm::vec3   mPosition;
};


/** This class contains and manages the visualisation data of the playing voices.
    It attempts to mirror the state of the SpatialSynth but on the message thread
    and adds aditional data for visualisation.
*/
class VisualPlayingVoicesState  : public ChangeBroadcaster
{
public:
    VisualPlayingVoicesState()
    {
    }

    void update()
    {
        if (mPlayingVoices.size() == 0)
            return;

        const int currTime = Time::getMillisecondCounter();

        std::remove_if(mPlayingVoices.begin(), mPlayingVoices.end(), [&](VisualVoiceData& a) {
            return (currTime - a.mStartTime) >= a.mFileLength;
        });

        sendSynchronousChangeMessage();
    }

    void addSound(int noteID, const SoundFileData& fileData, const glm::vec3& pos)
    {
        const double startTime = Time::getMillisecondCounter();
        VisualVoiceData v = { startTime, fileData.mFileLength, fileData.mName, noteID, pos };

        mPlayingVoices.push_back(v);
    }

    void updateSound(int noteID, const glm::vec3& pos)
    {
        for (auto& v : mPlayingVoices)
            if (v.mID == noteID)
                v.mPosition = pos;
    }

    void clear()
    {
        mPlayingVoices.clear();
    }

    std::vector<VisualVoiceData> mPlayingVoices;
};
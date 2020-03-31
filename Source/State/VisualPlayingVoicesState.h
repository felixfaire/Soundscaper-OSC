/*
  ==============================================================================

    VisualPlayingVoicesState.h
    Created: 30 Mar 2020 4:55:43pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include "vec3.hpp"
#include "glm.hpp"
#include "AudioDataState.h"

class VisualVoiceData
{
public:
    VisualVoiceData(int noteID, String fileName, uint32 startTime, uint32 fileLengthMillis, const glm::vec3& pos)
        : mID(noteID),
          mFileName(fileName),
          mStartTime(startTime),
          mFileLength(fileLengthMillis),
          mPosition(pos)
    {   
        mPositionHistory.push_back(pos);
    }

    void update(uint32 currTime, float deltaTimeSecs)
    {
        mProgress = (float)((double)(currTime - mStartTime) / (double)mFileLength);

        if (hasMoved())
        {
            const float delta = 0.99f * (float)mMaxHistory * deltaTimeSecs;

            for (size_t i = 0; i < mPositionHistory.size() - 1; ++i)
                mPositionHistory[i] = glm::mix(mPositionHistory[i], mPositionHistory[i + 1], glm::clamp(delta, 0.0f, 1.0f));
        }
    }

    void updatePosition(const glm::vec3& newPos)
    {
        mPosition = newPos;

        if (mPositionHistory.size() < mMaxHistory)
        {
            mPositionHistory.push_back(newPos);
        }
        else
        {
            for (int i = 0; i < mPositionHistory.size() - 1; ++i)
            {
                mPositionHistory[i] = mPositionHistory[i + 1];
            }

            mPositionHistory.back() = newPos;
        }
    }

    bool hasMoved() const { return mPositionHistory.size() > 1; }
    
    int         mID;
    String      mFileName;
    uint32      mStartTime;
    uint32      mFileLength;
    glm::vec3   mPosition;

    float                   mProgress = 0.0f;
    std::vector<glm::vec3>  mPositionHistory;
    int                     mMaxHistory = 32;
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
        mLastTime = Time::getMillisecondCounter();
    }

    void update()
    {
        if (mPlayingVoices.size() == 0)
            return;

        const int currTime = Time::getMillisecondCounter();
        const float deltaTime = (currTime - mLastTime) * 0.001f;

        for (auto& v : mPlayingVoices)
            v.update(currTime, deltaTime);

        mPlayingVoices.erase(std::remove_if(mPlayingVoices.begin(), mPlayingVoices.end(), [&](VisualVoiceData& a) {
            return (currTime - a.mStartTime) >= a.mFileLength;
        }), mPlayingVoices.end());

        sendSynchronousChangeMessage();

        mLastTime = currTime;
    }

    void addSound(int noteID, const SoundFileData& fileData, const glm::vec3& pos)
    {
        const uint32 startTime = Time::getMillisecondCounter();
        const uint32 millis = (uint32)(fileData.mFileLength * 1000.0);

        mPlayingVoices.emplace_back(noteID, fileData.mName, startTime, millis, pos);
    }

    void updateSound(int noteID, const glm::vec3& pos)
    {
        for (auto& v : mPlayingVoices)
            if (v.mID == noteID)
                v.updatePosition(pos);
    }

    void clear()
    {
        mPlayingVoices.clear();
    }

    uint32                       mLastTime;
    std::vector<VisualVoiceData> mPlayingVoices;
};

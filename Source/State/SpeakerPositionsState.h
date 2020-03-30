/*
  ==============================================================================

    SpeakerPositionState.h
    Created: 30 Mar 2020 3:15:03pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "vec3.hpp"


/** This class holds the data for the list of speakers and is responsible for sending
    messages on changes to the data.
*/
class SpeakerPositionsState  : public ChangeBroadcaster
{
public:
    SpeakerPositionsState()
    {
    }

    void addSpeaker(const glm::vec3& pos)
    {
        mSpeakerPositions.push_back(pos);
        sendChangeMessage();
    }

    void removeSpeaker()
    {
        mSpeakerPositions.pop_back();
        sendChangeMessage();
    }

    void setSpeakerPosition(int index, const glm::vec3& pos)
    {
        jassert(index < mSpeakerPositions.size());
        mSpeakerPositions[index] = pos;
        sendChangeMessage();
    }

    const std::vector<glm::vec3>& getPositions() const { return mSpeakerPositions; }
    const glm::vec3& getPosition(int i) const          { return mSpeakerPositions[i]; }

private:

    std::vector<glm::vec3>  mSpeakerPositions;

    friend class AppModelLoader;

};
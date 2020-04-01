/*
  ==============================================================================

    AudioLevelState.h
    Created: 30 Mar 2020 3:15:19pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AudioMonitorState : public ChangeBroadcaster
{
public:
    AudioMonitorState()
    {

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

        sendSynchronousChangeMessage();
    }

    const std::vector<float>&  getAudioLevels() const { return mAudioLevels; }

private:

    std::vector<float>                  mAudioLevels;

};
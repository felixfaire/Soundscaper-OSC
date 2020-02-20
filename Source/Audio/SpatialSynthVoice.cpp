/*
  ==============================================================================

    SpatialSynthVoice.cpp
    Created: 19 Feb 2020 10:38:33pm
    Author:  Felix Faire

  ==============================================================================
*/

#include "SpatialSynthVoice.h"
#include "geometric.hpp"


SpatialSynthVoice::SpatialSynthVoice() {}
SpatialSynthVoice::~SpatialSynthVoice() {}

void SpatialSynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    currentSampleRate = newRate;
}

void SpatialSynthVoice::setNumSpeakerOutputs (int numSpeakers)
{
    channelAmplitudes.resize(numSpeakers, 1.0f);
}

bool SpatialSynthVoice::isVoiceActive() const
{
    return getCurrentlyPlayingNote() >= 0;
}

void SpatialSynthVoice::clearCurrentNote()
{
    currentlyPlayingNote = -1;
    currentlyPlayingSound = nullptr;
}

bool SpatialSynthVoice::wasStartedBefore (const SpatialSynthVoice& other) const noexcept
{
    return noteOnTime < other.noteOnTime;
}

void SpatialSynthVoice::renderNextBlock (AudioBuffer<double>& outputBuffer,
                                        int startSample, int numSamples)
{
    AudioBuffer<double> subBuffer (outputBuffer.getArrayOfWritePointers(),
                                   outputBuffer.getNumChannels(),
                                   startSample, numSamples);

    tempBuffer.makeCopyOf (subBuffer, true);
    renderNextBlock (tempBuffer, 0, numSamples);
    subBuffer.makeCopyOf (tempBuffer, true);
}

void SpatialSynthVoice::positionChanged (const glm::vec3& newPosition)
{
    position = newPosition;
    needsDBAPUpdate = true;
}

void SpatialSynthVoice::updateDBAPAmplitudes(const std::vector<glm::vec3>& positions)
{
    for (int i = 0; i < channelAmplitudes.size(); ++i)
        channelAmplitudes[i] = std::min(1.0f, 0.1f / glm::distance(positions[i], position));
        
    needsDBAPUpdate = false;
}

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
    return getCurrentNoteID() >= 0;
}

void SpatialSynthVoice::clearCurrentNote()
{
    currentNoteID = -1;
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
    const float rolloffDb = 6.0f;
    const float rolloffPowFactor = std::log(std::pow(10.0f, (rolloffDb / 20.0f)))/std::log(2.0f);
    float k = 0.0f;   // Scaling coeff
    float invk2 = 0.0f; // Inverse square of k
    
    for (int i = 0; i < channelAmplitudes.size(); ++i)
    {
        const float dist = glm::distance(positions[i], position);
        const float unNormalized = std::pow(dist, 0.5f * rolloffPowFactor);
        channelAmplitudes[i] = unNormalized;
        invk2 += 1.0f / (unNormalized * unNormalized);
    }
    
    k = std::sqrt(1.0f / invk2);
    
    // normalize amplitudes
    for (int i = 0; i < channelAmplitudes.size(); ++i)
        channelAmplitudes[i] = std::min(1.0f, k / channelAmplitudes[i]);
    
    needsDBAPUpdate = false;
}

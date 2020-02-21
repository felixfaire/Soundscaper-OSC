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
    mCurrentSampleRate = newRate;
}

void SpatialSynthVoice::setNumSpeakerOutputs (int numSpeakers)
{
    mChannelAmplitudes.resize(numSpeakers, 1.0f);
}

bool SpatialSynthVoice::isVoiceActive() const
{
    return getCurrentNoteID() >= 0;
}

void SpatialSynthVoice::clearCurrentNote()
{
    mCurrentNoteID = -1;
    mCurrentlyPlayingSound = nullptr;
}

bool SpatialSynthVoice::wasStartedBefore (const SpatialSynthVoice& other) const noexcept
{
    return mNoteOnTime < other.mNoteOnTime;
}

void SpatialSynthVoice::renderNextBlock (AudioBuffer<double>& outputBuffer,
                                        int startSample, int numSamples)
{
    AudioBuffer<double> subBuffer (outputBuffer.getArrayOfWritePointers(),
                                   outputBuffer.getNumChannels(),
                                   startSample, numSamples);

    mTempBuffer.makeCopyOf (subBuffer, true);
    renderNextBlock (mTempBuffer, 0, numSamples);
    subBuffer.makeCopyOf (mTempBuffer, true);
}

void SpatialSynthVoice::positionChanged (const glm::vec3& newPosition)
{
    mPosition = newPosition;
    mNeedsDBAPUpdate = true;
}

void SpatialSynthVoice::updateDBAPAmplitudes(const std::vector<glm::vec3>& positions)
{
    const float rolloffDb = 6.0f;
    const float rolloffPowFactor = std::log(std::pow(10.0f, (rolloffDb / 20.0f)))/std::log(2.0f);
    float k = 0.0f;     // Scaling coeff
    float invk2 = 0.0f; // Inverse square of k
    
    for (int i = 0; i < mChannelAmplitudes.size(); ++i)
    {
        const float dist = glm::distance(positions[i], mPosition);
        const float unNormalized = std::pow(dist, 0.5f * rolloffPowFactor);
        mChannelAmplitudes[i] = unNormalized;
        invk2 += 1.0f / (unNormalized * unNormalized);
    }
    
    k = std::sqrt(1.0f / invk2);
    
    // normalize amplitudes
    for (int i = 0; i < mChannelAmplitudes.size(); ++i)
        mChannelAmplitudes[i] = std::min(1.0f, k / mChannelAmplitudes[i]);
    
    // test
    for (int i = 0; i < mChannelAmplitudes.size(); ++i)
    {
        const float dist = glm::distance(positions[i], mPosition);
        mChannelAmplitudes[i] = std::min(1.0f, 0.1f / dist);
    }
    mNeedsDBAPUpdate = false;
}

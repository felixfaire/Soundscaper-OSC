/*
  ==============================================================================

    SpatialSynthVoice.cpp
    Created: 19 Feb 2020 10:38:33pm
    Author:  Felix Faire

  ==============================================================================
*/

#include "SpatialSynthVoice.h"


SpatialSynthVoice::SpatialSynthVoice() {}
SpatialSynthVoice::~SpatialSynthVoice() {}

void SpatialSynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    currentSampleRate = newRate;
}

void SpatialSynthVoice::setOutputInfo (int numChannels, const double newRate)
{
    currentSampleRate = newRate;
    channelAmplitudes.resize(numChannels, 1.0f);
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

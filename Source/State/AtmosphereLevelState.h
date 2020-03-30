/*
  ==============================================================================

    AtmosphereLevelState.h
    Created: 30 Mar 2020 3:24:32pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

class AtmosphereLevelState : public ChangeBroadcaster
{
public:
    AtmosphereLevelState()
    {

    }

    void resetAmplitudes(int numAmplitudes)
    {
        mSoundAtmosphereAmplitudes = std::vector<float>(numAmplitudes, 0.0f);
        sendChangeMessage();
    }

    void setSoundAtmosphereAmplitude(int index, float newAmplitude)
    {
        jassert(index < mSoundAtmosphereAmplitudes.size());

        if (index >= mSoundAtmosphereAmplitudes.size())
            return;

        mSoundAtmosphereAmplitudes[index] = newAmplitude;
        sendChangeMessage();
    }

    const std::vector<float>&   getSoundAtmosphereAmpitudes() const { return mSoundAtmosphereAmplitudes; }
    float                       getSoundAtmosphereAmpitude(int i) const                 { return mSoundAtmosphereAmplitudes[i]; }


private:

    std::vector<float>                  mSoundAtmosphereAmplitudes;

};
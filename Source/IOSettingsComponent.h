/*
  ==============================================================================

    IOSettingsComponent.h
    Created: 12 Feb 2020 8:44:56pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class IOSettingsComponent    : public Component
{
public:
    IOSettingsComponent(AudioDeviceManager& deviceManager)
    {
        mDeviceSelector.reset(new AudioDeviceSelectorComponent(deviceManager,
                                                               0, 0,
                                                               1, 10,
                                                               false, false,
                                                               false,
                                                               false));
                                                               
        addAndMakeVisible(*mDeviceSelector);
    }

    ~IOSettingsComponent()
    {
    }

    void paint (Graphics& g) override
    {

    }

    void resized() override
    {
        mDeviceSelector->setBounds(getBounds());
    }

private:

    std::unique_ptr<AudioDeviceSelectorComponent> mDeviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IOSettingsComponent)
};

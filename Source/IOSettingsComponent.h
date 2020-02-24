/*
  ==============================================================================

    IOSettingsComponent.h
    Created: 12 Feb 2020 8:44:56pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OSC/OSCSettings.h"

//==============================================================================
/*
*/
class IOSettingsComponent    : public Component
{
public:
    IOSettingsComponent(AudioDeviceManager& deviceManager)
    {
        mAudioSettingsLabel.reset(new Label("AudioSettingsLabel", "Audio Settings"));
        mAudioSettingsLabel->setJustificationType(Justification(Justification::Flags::centred));
        mAudioSettingsLabel->setFont(mAudioSettingsLabel->getFont().withHeight(0.6f * (float)mLabelHeight));
        
        mOSCSettingsLabel.reset(new Label("OSCSettingsLabel", "OSC Settings"));
        mOSCSettingsLabel->setJustificationType(Justification(Justification::Flags::centred));
        mOSCSettingsLabel->setFont(mOSCSettingsLabel->getFont().withHeight(0.6f * (float)mLabelHeight));
        
        mAudioSettings.reset(new AudioDeviceSelectorComponent(deviceManager,
                                                               0, 0,
                                                               1, 10,
                                                               false, false,
                                                               false,
                                                               false));
        
        mOSCSettingsComponent.reset(new OSCSettingsComponent());

        addAndMakeVisible(*mAudioSettingsLabel);
        addAndMakeVisible(*mOSCSettingsLabel);
        addAndMakeVisible(*mAudioSettings);
        addAndMakeVisible(*mOSCSettingsComponent);
    }

    ~IOSettingsComponent()
    {
    }

    void paint (Graphics& g) override
    {

    }

    void resized() override
    {
        auto b = getLocalBounds();
        mAudioSettingsLabel->setBounds(b.removeFromTop(mLabelHeight));
        
        int settingsHeight = mAudioSettings->getItemHeight() * 10;
        mAudioSettings->setBounds(b.removeFromTop(settingsHeight));
        
        mOSCSettingsLabel->setBounds(b.removeFromTop(mLabelHeight));
        mOSCSettingsComponent->setBounds(b);
    }

private:

    int                                             mLabelHeight = 50;
    
    std::unique_ptr<Label>                          mAudioSettingsLabel;
    std::unique_ptr<Label>                          mOSCSettingsLabel;
    
    std::unique_ptr<AudioDeviceSelectorComponent>   mAudioSettings;
    std::unique_ptr<OSCSettingsComponent>           mOSCSettingsComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IOSettingsComponent)
};

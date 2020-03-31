/*
  ==============================================================================

    IOSettingsComponent.h
    Created: 12 Feb 2020 8:44:56pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../UIElements/MinimalLookAndFeel.h"
#include "../OSC/OSCSettingsComponent.h"


class LabelledSettingsContainer : public Component
{
public:
    LabelledSettingsContainer(const std::string& title, Component* content)
        : mContent(content)
    {
        jassert(content != nullptr);

        mLabel.reset(new Label("Label", title));
        //mLabel->setJustificationType(Justification(Justification::Flags::centred));
        mLabel->setFont(mLabel->getFont().withHeight(0.5f * (float)mLabelHeight));

        addAndMakeVisible(*mLabel);
        addAndMakeVisible(*mContent);
    }

    void paint(Graphics& g) override
    {
        auto b = getLocalBounds().reduced(1).toFloat();
        MinimalLookAndFeel::drawPanelBackground(g, b);
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced(5);

        mLabel->setBounds(b.removeFromTop(mLabelHeight));

        jassert(mContent != nullptr);

        if (mContent != nullptr)
            mContent->setBounds(b);

    }

private:
    
    Component*             mContent;
    int                    mLabelHeight = 50;
    std::unique_ptr<Label> mLabel;
};

//==============================================================================
/*
*/
class IOSettingsComponent    : public Component
{
public:
    IOSettingsComponent(AppModel& m, AudioDeviceManager& deviceManager)
    {
        mAudioSettings.reset(new AudioDeviceSelectorComponent(deviceManager,
                                                               0, 0,
                                                               1, 10,
                                                               false, false,
                                                               false,
                                                               false));
        
        mOSCSettings.reset(new OSCSettingsComponent(m.mOSCReciever));

        mAudioSettingsContainer.reset(new LabelledSettingsContainer("Audio Settings", mAudioSettings.get()));
        mOSCSettingsContainer.reset(new LabelledSettingsContainer("OSC Settings", mOSCSettings.get()));

        addAndMakeVisible(*mAudioSettingsContainer);
        addAndMakeVisible(*mOSCSettingsContainer);
    }

    ~IOSettingsComponent()
    {
    }

    void resized() override
    {
        auto b = getLocalBounds();
        int settingsHeight = mAudioSettings->getItemHeight() * 10 + 50;
        mAudioSettingsContainer->setBounds(b.removeFromTop(settingsHeight));
        b.removeFromTop(10);
        mOSCSettingsContainer->setBounds(b);
    }

private:

    
    std::unique_ptr<LabelledSettingsContainer>      mAudioSettingsContainer;
    std::unique_ptr<LabelledSettingsContainer>      mOSCSettingsContainer;
    
    std::unique_ptr<AudioDeviceSelectorComponent>   mAudioSettings;
    std::unique_ptr<OSCSettingsComponent>           mOSCSettings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IOSettingsComponent)
};

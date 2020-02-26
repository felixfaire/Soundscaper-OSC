/*
  ==============================================================================

    SpeakerEditorPopupComponent.h
    Created: 26 Feb 2020 10:29:09pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LabelSlider : public Component
{
public:
    LabelSlider(const String& name)
    {
        mLabel.setJustificationType(Justification::centredRight);
        mLabel.setText(name, NotificationType::dontSendNotification);
        mLabel.setMinimumHorizontalScale(1.0f);
        
        addAndMakeVisible(mLabel);
        addAndMakeVisible(mSlider);
    }
    
    void resized()
    {
        auto b = getLocalBounds();
        auto labelWidth = mLabel.getFont().getStringWidth(mLabel.getText());
        mLabel.setBounds(b.removeFromLeft(labelWidth + 10));
        mSlider.setBounds(b);
    }
    
    Slider  mSlider;
    Label   mLabel;
};

class SpeakerEditorPopupComponent  : public Component,
                                public ChangeBroadcaster
{
public:
    SpeakerEditorPopupComponent(const glm::vec3& pos)
    {
        std::vector<String> names = { "x: ", "y: ", "z: " };
        
        for (int i = 0; i < 3; ++i)
        {
            mPositionSliders[i].reset(new LabelSlider(names[i]));
            mPositionSliders[i]->mSlider.setRange(-10.0, 10.0);
            mPositionSliders[i]->mSlider.setSliderStyle(Slider::LinearHorizontal);
            mPositionSliders[i]->mSlider.setValue(pos[i]);
            mPositionSliders[i]->mSlider.setNumDecimalPlacesToDisplay(2);
            mPositionSliders[i]->mSlider.onValueChange = [this] () {
                sendChangeMessage();
            };
            
            addAndMakeVisible(mPositionSliders[i].get());
        }
    }
    
    ~SpeakerEditorPopupComponent()
    {
        dispatchPendingMessages();
    }
    
    void resized()
    {
        auto b = getLocalBounds();
        const auto step = b.getHeight() / 3;
        
        for (int i = 0; i < 3; ++i)
            mPositionSliders[i]->setBounds(b.removeFromTop(step).reduced(5));
    }
    
    glm::vec3 getPosition()
    {
        return glm::vec3((float)mPositionSliders[0]->mSlider.getValue(),
                         (float)mPositionSliders[1]->mSlider.getValue(),
                         (float)mPositionSliders[2]->mSlider.getValue());
    }
    
private:

    std::unique_ptr<LabelSlider> mPositionSliders [3];
    
};

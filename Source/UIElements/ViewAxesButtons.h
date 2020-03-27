/*
  ==============================================================================

    ViewAxesButtons.h
    Created: 27 Mar 2020 1:59:50pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "../Utils/ViewAxes.h"

class ViewAxesButtons : public Component,
                        public Button::Listener
{
public:
    ViewAxesButtons()
        : mXZButton("Top", "XZ"),
          mXYButton("Front", "XY"),
          mZYButton("Side", "ZY")
    {
        mXZButton.setRadioGroupId(12345);
        mXYButton.setRadioGroupId(12345);
        mZYButton.setRadioGroupId(12345);
        
        mXZButton.setClickingTogglesState(true);
        mXYButton.setClickingTogglesState(true);
        mZYButton.setClickingTogglesState(true);
        
        mXZButton.addListener(this);
        mXYButton.addListener(this);
        mZYButton.addListener(this);
        
        addAndMakeVisible(mXZButton);
        addAndMakeVisible(mXYButton);
        addAndMakeVisible(mZYButton);
        
        mXZButton.setToggleState(true, dontSendNotification);
    }
    
    void resized() override
    {
        auto b = getLocalBounds();
        const float step = (float)b.getWidth() / 3.0f;
        
        mXZButton.setBounds(b.removeFromLeft(step).reduced(5));
        mXYButton.setBounds(b.removeFromLeft(step).reduced(5));
        mZYButton.setBounds(b.reduced(5));
    }

    std::function<void(ViewAxes)> onViewAxesChanged;

private:

    void buttonClicked(Button* b) override
    {
        if (onViewAxesChanged == nullptr || b->getToggleState() == false)
            return;
        
        if (b == &mXZButton)
            onViewAxesChanged(ViewAxes::XZ);
            
        if (b == &mXYButton)
            onViewAxesChanged(ViewAxes::XY);
        
        if (b == &mZYButton)
            onViewAxesChanged(ViewAxes::ZY);
    }

    TextButton  mXZButton;
    TextButton  mXYButton;
    TextButton  mZYButton;
    
};

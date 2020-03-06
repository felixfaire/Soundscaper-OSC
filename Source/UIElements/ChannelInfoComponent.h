/*
  ==============================================================================

    ChannelInfoComponent.h
    Created: 6 Mar 2020 3:23:12pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

class ChannelInfoComponent : public Component
{
public:
    ChannelInfoComponent(int index)
    {
        mName.reset(new Label(std::to_string(index)));
        
        addAndMakeVisible(*index);
    }
    
    void paint(Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        
        g.setColour(Colour::greyLevel(0.1f));
        g.fillRoundedRectangle(b, 4.0f);
    }
    
    void resized()
    {
        auto b = getLocalBounds();
        
        mName->setBounds(b);
    }
private:

    std::unique_ptr<Label> mName;
    
};

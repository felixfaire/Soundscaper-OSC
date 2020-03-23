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
        const auto sindex = std::to_string(index + 1);
        mName.reset(new Label("ch_" + sindex, sindex));
        mName->setJustificationType(Justification(Justification::Flags::centred));
        
        addAndMakeVisible(*mName);
    }
    
    void paint(Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        
        if (!isEnabled())
            mLevel = 0.0f;
        
        const float bk = isEnabled() ? 0.15f : 0.1f;
        
        g.setColour(Colour::greyLevel(bk));
        g.fillRoundedRectangle(b, 4.0f);
        
        if (isEnabled())
        {
            b.reduced(2.0f);
            b.setWidth(b.getWidth() * mLevel * 2.0f);
            g.setColour(Colours::lightgreen);
            g.fillRoundedRectangle(b, 4.0f);
        }
    }
    
    void resized() override
    {
        auto b = getLocalBounds();
        
        mName->setBounds(b);
    }
    
    void setLevel(float newLevel)
    {
        mLevel = newLevel;
        repaint();
    }
    
    
private:

    float mLevel = 0.0f;
    std::unique_ptr<Label> mName;
    
};

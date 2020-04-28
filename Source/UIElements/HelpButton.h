/*
  ==============================================================================

    HelpButton.h
    Created: 7 Apr 2020 5:59:28pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IconPaths.h"

class HelpInfoComponent : public Component
{
public:
    HelpInfoComponent(const TextLayout& text)
        : mTextLayout(text)
    {
        
    }
    
    void paint(Graphics& g) override
    {
        mTextLayout.draw(g, getLocalBounds().reduced(20).toFloat());
    }
    
    TextLayout mTextLayout;
};


class HelpButton : public Button
{
public:
    HelpButton()
        : Button("HelpButton")
    {
        mHelpIcon = IconPaths::getHelpPath();
        
        onClick = [this]() {
            
            const auto maxWidth = 350.0f;
            const auto margin = 20;
            
            Colour c = Colours::white;
            
            AttributedString text;
            text.append(mTitle, Font(20.0f), c);
            text.append("\n");
            text.append("\n");
            text.append(mText, Font(17.0f), c);
            text.setLineSpacing(1.6f);
            
            TextLayout tl;
            tl.createLayout(text, maxWidth);
            
            auto* helpInfo = new HelpInfoComponent(tl);
            helpInfo->setSize(tl.getWidth() + margin * 2, tl.getHeight() + margin * 2);

            auto& cb = CallOutBox::launchAsynchronously(helpInfo, getScreenBounds(), getTopLevelComponent() );
            cb.setDismissalMouseClicksAreAlwaysConsumed(true);
            
        };
    }
    
    void paintButton (Graphics& g,
                      bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override
    {
        auto b = getLocalBounds().reduced(1).toFloat();
        
        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour(Colours::white.withAlpha(0.1f));
            g.fillEllipse(b);
        }
                
        g.setColour(Colour::greyLevel(0.9f));
        
        const float thickness = 1.0f;
        const int   inset = (int)(getWidth() * 0.2f);
        g.drawEllipse(b, thickness);
        
        AffineTransform t = RectanglePlacement().getTransformToFit(mHelpIcon.getBounds(), b.reduced(inset));
        g.strokePath(mHelpIcon,
                     PathStrokeType(thickness,
                     juce::PathStrokeType::JointStyle::mitered,
                     juce::PathStrokeType::EndCapStyle::rounded),
                     t);
    }
    
    void setTitle(const String& newTitle)
    {
        mTitle = newTitle;
    }
    
    void setText(const String& newText)
    {
        mText = newText;
    }

    
private:

    String  mTitle;
    String  mText;
    
    Path    mHelpIcon;

};

/*
  ==============================================================================

    MinimalLookAndFeel.h
    Created: 12 Feb 2020 10:30:24pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MinimalLookAndFeel    : public LookAndFeel_V4
{
public:
    MinimalLookAndFeel()
    {

    }

    ~MinimalLookAndFeel()
    {
    }
    
    // ===== Text Button ========================
    
    // ===== File Component ========================
    
    void layoutFilenameComponent (FilenameComponent& filenameComp,
                                  ComboBox* filenameBox, Button* browseButton) override
    {
        browseButton->setSize (80, filenameComp.getHeight());

        if (auto* tb = dynamic_cast<TextButton*> (browseButton))
            tb->changeWidthToFitText();

        browseButton->setTopRightPosition (filenameComp.getWidth(), 0);

        const int margin = 5;
        filenameBox->setBounds (0, 0, browseButton->getX() - margin, filenameComp.getHeight());
    }
    
    // ===== TABBED COMPONENT ========================
    
    int getTabButtonBestWidth (TabBarButton& b, int tabDepth) override
    {
        const auto& bar = b.getTabbedButtonBar();
        return bar.getWidth() / bar.getNumTabs();
    }
    
    Typeface::Ptr getTypefaceForFont(const Font& f) override
    {
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::RalewayRegular_ttf, BinaryData::RalewayRegular_ttfSize);
        return myFont;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinimalLookAndFeel)
};

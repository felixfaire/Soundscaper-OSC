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
    
    void drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        const Rectangle<int> activeArea (button.getActiveArea());

        const Colour bkg (button.getTabBackgroundColour());
        const float textAlphaMult = button.getToggleState() ? 1.0f : 0.3f;
    
//        if (button.getToggleState())
//        {
            g.setColour (bkg);
//        }
//        else
//        {
//            Point<int> p1, p2;
//            p1 = activeArea.getTopLeft();
//            p2 = activeArea.getBottomLeft();
//
//            g.setGradientFill (ColourGradient (bkg.brighter (0.2f), p1.toFloat(),
//                                               bkg.darker (0.1f),   p2.toFloat(), false));
//        }

        //g.fillRect (activeArea);

        g.setColour (button.findColour (TabbedButtonBar::tabOutlineColourId));

        Rectangle<int> r (activeArea);
        r.removeFromBottom(5);

        // Text Underline
        if (button.getToggleState())
            g.fillRect(r.removeFromBottom(2).reduced(40, 0));

        float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;
        alpha *= textAlphaMult;

        Colour col (bkg.contrasting().withMultipliedAlpha (alpha));

//        if (TabbedButtonBar* bar = button.findParentComponentOfClass<TabbedButtonBar>())
//        {
//            TabbedButtonBar::ColourIds colID = button.isFrontTab() ? TabbedButtonBar::frontTextColourId
//                                                                   : TabbedButtonBar::tabTextColourId;
//
//            if (bar->isColourSpecified (colID))
//                col = bar->findColour (colID);
//            else if (isColourSpecified (colID))
//                col = findColour (colID);
//        }

        const Rectangle<float> area (button.getTextArea().toFloat());

        TextLayout textLayout;
        createTabTextLayout (button, area.getWidth(), area.getHeight(), col, textLayout);

        textLayout.draw (g, area);
    }
    
    void drawTabbedButtonBarBackground(TabbedButtonBar&, Graphics&) override
    {}
    
    void drawTabAreaBehindFrontButton (TabbedButtonBar&, Graphics&, int w, int h) override
    {}
    
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

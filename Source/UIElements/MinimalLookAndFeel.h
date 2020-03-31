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

    static Colour getSoundColour() { return (Colour(0xcc1b9f89)); }

    static void drawDarkPanelBackground(Graphics& g, Rectangle<float> bounds)
    {
        g.setColour(Colour::greyLevel(0.05f));
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(Colour::greyLevel(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }

    static void drawPanelBackground(Graphics& g, Rectangle<float> bounds)
    {
        g.setColour(Colours::white.withAlpha(0.05f));
        g.fillRoundedRectangle(bounds, 4.0f);
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

    // ===== LABEL ==========================================================

    void drawLabel (Graphics& g, Label& label) override
    {
        if (label.isEditable() && label.isEnabled())
        {
            auto b = label.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

            auto bkColour = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::widgetBackground);
            
            if (label.isMouseOver())
                bkColour = bkColour.contrasting (0.05f);
            
            g.setColour(bkColour);
            g.fillRoundedRectangle(b, 4.0f);

            g.setColour(label.findColour(Label::outlineColourId));
            g.drawRoundedRectangle(b, 4.0f, 1.0f);
        }

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font (getLabelFont (label));

            g.setColour (label.findColour (Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (font);

            auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

            g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                jmax (1, (int) (textArea.getHeight() / font.getHeight())),
                label.getMinimumHorizontalScale());
        }
    }

    // ===== FONTS ==========================================================
    
    Typeface::Ptr getTypefaceForFont(const Font& f) override
    {
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::RalewayRegular_ttf, BinaryData::RalewayRegular_ttfSize);
        return myFont;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MinimalLookAndFeel)
};

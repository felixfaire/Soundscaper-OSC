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

    static Colour getSoundColour() { return(Colour(0xcc1b9f89)); }
    static Colour getLevelColour() { return(Colour(0xcc1b9f89)); }

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

    static void drawFileListItemBackground(Graphics& g, Rectangle<int> localBounds)
    {
        localBounds.reduce(1, 5);

        g.setColour(Colour::greyLevel(0.4f));
        g.drawRoundedRectangle(localBounds.toFloat(), 4.0f, 1.0f);
    }
    
    // ===== Text Button ========================
    
    // ===== File Component ========================
    
    void layoutFilenameComponent(FilenameComponent& filenameComp,
                                  ComboBox* filenameBox, Button* browseButton) override
    {
        browseButton->setSize(80, filenameComp.getHeight());

        if (auto* tb = dynamic_cast<TextButton*>(browseButton))
            tb->changeWidthToFitText();

        browseButton->setTopRightPosition(filenameComp.getWidth(), 0);

        const int margin = 5;
        filenameBox->setBounds(0, 0, browseButton->getX() - margin, filenameComp.getHeight());
    }
    
    // ===== TABBED COMPONENT ========================
    
    void drawTabButton(TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        float alpha = button.getToggleState() ? 1.0f : 0.3f;
        alpha *= button.isEnabled() ?((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

        Colour col = Colour::greyLevel(0.9f).withAlpha(alpha);
        const Rectangle<float> area(button.getTextArea().toFloat());

        // Draw text
        TextLayout textLayout;
        createTabTextLayout(button, area.getWidth(), area.getHeight(), col, textLayout);
        textLayout.draw(g, area);
        
        // Text Underline
        Rectangle<int> r = button.getActiveArea();
        r.removeFromBottom(5);
        g.setColour(col);
        
        if (button.getToggleState())
            g.fillRect(r.removeFromBottom(2).reduced(35, 0));
    }
    
    void drawTabbedButtonBarBackground(TabbedButtonBar&, Graphics&) override
    {}
    
    void drawTabAreaBehindFrontButton(TabbedButtonBar&, Graphics&, int w, int h) override
    {}
    
    int getTabButtonBestWidth(TabBarButton& b, int tabDepth) override
    {
        const auto& bar = b.getTabbedButtonBar();
        return bar.getWidth() / bar.getNumTabs();
    }

    // ===== LABEL ==========================================================

    void drawLabel(Graphics& g, Label& label) override
    {
        if (label.isEditable() && label.isEnabled())
        {
            auto b = label.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

            auto bkColour = getCurrentColourScheme().getUIColour(ColourScheme::UIColour::widgetBackground);
            
            if (label.isMouseOver())
                bkColour = bkColour.contrasting(0.05f);
            
            g.setColour(bkColour);
            g.fillRoundedRectangle(b, 4.0f);

            g.setColour(label.findColour(Label::outlineColourId));
            g.drawRoundedRectangle(b, 4.0f, 1.0f);
        }

        if (!label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font(getLabelFont(label));

            g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
            g.setFont(font);

            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

            g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                jmax(1,(int)(textArea.getHeight() / font.getHeight())),
                label.getMinimumHorizontalScale());
        }
    }

    // ===== COMBOBOX =======================================================

    void positionComboBoxText(ComboBox& box, Label& label) override
    {
        label.setBounds(1, 1,
            box.getWidth() - 30,
            box.getHeight() - 2);

        label.setMinimumHorizontalScale(1.0f);
        label.setFont(getComboBoxFont(box));
    }

    // ===== SLIDER =========================================================

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        auto outline = Colours::white.withAlpha(slider.isMouseOverOrDragging() ? 0.15f : 0.1f);

        auto fill    = getSoundColour();//slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(4.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
            arcRadius, arcRadius, 0.0f,
            rotaryStartAngle, rotaryEndAngle, true);

        g.setColour(outline);
        g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                arcRadius, arcRadius, 0.0f,
                rotaryStartAngle, toAngle, true);

            g.setColour(fill);
            g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        auto thumbWidth = lineW * 1.0f;
        g.setColour(slider.isMouseOverOrDragging() ? fill : outline);

        const auto dotRadius = arcRadius - lineW * 1.5f;
        const auto dotWidth = thumbWidth * 0.8f;
        Point<float> dotPoint(bounds.getCentreX() + dotRadius * std::cos(toAngle - MathConstants<float>::halfPi),
                                bounds.getCentreY() + dotRadius * std::sin(toAngle - MathConstants<float>::halfPi));
        g.fillEllipse(Rectangle<float>(dotWidth, dotWidth).withCentre(dotPoint));
        
    }

    // ===== TOOLTIP ========================================================

    static TextLayout layoutTooltipText(const String& text, Colour colour) noexcept
    {
        const float tooltipFontSize = 16.0f;
        const int maxToolTipWidth = 400;

        AttributedString s;
        s.setJustification(Justification::centred);
        s.append(text, Font(tooltipFontSize, Font::bold), colour);

        TextLayout tl;
        tl.createLayoutWithBalancedLineLengths (s, (float)maxToolTipWidth);
        return tl;
    }

    Rectangle<int> getTooltipBounds (const String& tipText, Point<int> screenPos, Rectangle<int> parentArea) override
    {
        TextLayout tl = layoutTooltipText(tipText, Colours::black);

        auto w = (int) (tl.getWidth() + 14.0f);
        auto h = (int) (tl.getHeight() + 6.0f);

        return Rectangle<int> (screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 12) : screenPos.x + 24,
            screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 6)  : screenPos.y + 6,
            w, h)
            .constrainedWithin (parentArea);
    }

    void drawTooltip(Graphics& g, const String& text, int width, int height) override
    {
        Rectangle<int> bounds(width, height);
        auto cornerSize = 4.0f;

        g.setColour(findColour(TooltipWindow::backgroundColourId));
        g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

        layoutTooltipText(text, findColour(TooltipWindow::textColourId))
            .draw(g, { static_cast<float>(width), static_cast<float>(height) });
    }

    // ===== FONTS ==========================================================
    
    Typeface::Ptr getTypefaceForFont(const Font& f) override
    {
        static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::RalewayRegular_ttf, BinaryData::RalewayRegular_ttfSize);
        return myFont;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinimalLookAndFeel)
};

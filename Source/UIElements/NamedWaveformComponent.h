/*
  ==============================================================================

    NamedWaveformComponent.h
    Created: 2 Mar 2020 12:56:07pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MinimalLookAndFeel.h"



class NamedWaveformComponent  : public Component
{
public:
    NamedWaveformComponent()
    {
        mLabel.reset(new Label("NameLabel", ""));
        mLabel->setColour(Label::ColourIds::textColourId, Colour::greyLevel(0.9f));
        mLabel->setFont(mLabel->getFont().withHeight(18.0f));
        mLabel->setInterceptsMouseClicks(true, true);

        addAndMakeVisible(*mLabel);
    }
    
    void paint(Graphics& g) override
    {        
        drawWaveform(g, getLocalBounds().toFloat());
    }

    void resized() override
    {
        mLabel->setBounds(getLocalBounds().reduced(10, 0));
        mTextWidth = (float)mLabel->getFont().getStringWidth(mLabel->getText());
    }
    
    void setData(const SoundFileData* newData) 
    { 
        mData = newData; 
        mLabel->setText(String(mData->mIndex) + String(": ") + mData->mName, dontSendNotification);
        updateTooltip();
    }
    
private:

    void updateTooltip()
    {
        mLabel->setTooltip("OSC string id: \n" + String("\"") + mData->mOSCAddress + String("\""));
    }

    void drawWaveform(Graphics& g, Rectangle<float> b)
    {
        jassert(mData != nullptr);

        if (mData == nullptr)
            return;

        Path waveform;
        waveform.startNewSubPath(b.getX(), b.getCentreY());

        const float step = b.getWidth() / mData->mWaveformSize;
        const float height = b.getHeight() * 0.5f;

        // forward along top
        for (int i = 0; i < mData->mWaveformSize; ++i)
            waveform.lineTo(b.getX() + i * step, b.getCentreY() + mData->mWaveform[i] * height);

        waveform.lineTo(b.getRight(), b.getCentreY());

        // Back along bottom
        for (int i = mData->mWaveformSize - 1; i >= 0; --i)
            waveform.lineTo(b.getX() + i * step, b.getCentreY() - mData->mWaveform[i] * height);

        waveform.closeSubPath();

        g.setColour(Colours::black.withAlpha(0.3f));

        const float start = b.getX() + mTextWidth;
        const float end = start + 100.0f;

        ColourGradient grad(MinimalLookAndFeel::getSoundColour().withAlpha(0.13f), { start, b.getCentreY() },
                            MinimalLookAndFeel::getSoundColour().withAlpha(0.3f), { end, b.getCentreY() }, false);
        g.setGradientFill(grad);
        g.fillPath(waveform);
    }

    const SoundFileData* mData = nullptr;

    std::unique_ptr<Label> mLabel;
    float                  mTextWidth;

};

/*
  ==============================================================================

    AudioFileComponent.h
    Created: 2 Mar 2020 12:56:07pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MinimalLookAndFeel.h"

class AudioFileComponent  : public Component
{
public:
    AudioFileComponent(const SoundFileData& data)
        : mData(&data)
    {
        mLabel.reset(new Label("NameLabel", data.mName));
        updateTooltip();
        mLabel->setColour(Label::ColourIds::textColourId, Colour::greyLevel(0.9f));
        mLabel->setFont(mLabel->getFont().withHeight(15.0f));
        mLabel->setInterceptsMouseClicks(true, true);

        addAndMakeVisible(*mLabel);
    }
    
    void paint(Graphics& g) override
    {
        jassert(mData != nullptr);
        
        auto b = getLocalBounds();
        b.reduce(1, 5);

        g.setColour(Colour::greyLevel(0.4f));
        g.drawRoundedRectangle(b.toFloat(), 4.0f, 1.0f);

        drawWaveform(g, b.reduced(0, 2).toFloat());
    }

    void resized() override
    {
        mLabel->setBounds(getLocalBounds().reduced(10, 0));
        mTextWidth = (float)mLabel->getFont().getStringWidth(mLabel->getText());
    }
    
    void setData(const SoundFileData* newData) 
    { 
        mData = newData; 
        mLabel->setText(mData->mName, dontSendNotification);
        updateTooltip();
    }
    
private:

    void updateTooltip()
    {
        mLabel->setTooltip("OSC Adress: " + mData->mOSCAddress);
    }

    void drawWaveform(Graphics& g, Rectangle<float> b)
    {
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

        ColourGradient grad(MinimalLookAndFeel::getSoundColour().withAlpha(0.1f), { start, b.getCentreY() },
                            MinimalLookAndFeel::getSoundColour().withAlpha(0.3f), { end, b.getCentreY() }, false);
        g.setGradientFill(grad);
        g.fillPath(waveform);
    }

    const SoundFileData* mData;

    std::unique_ptr<Label> mLabel;
    float                  mTextWidth;

};

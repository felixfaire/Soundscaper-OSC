/*
  ==============================================================================

    AudioFileComponent.h
    Created: 2 Mar 2020 12:56:07pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AudioFileComponent  : public Component
{
public:
    AudioFileComponent(const SoundFileData& data)
        : mData(&data)
    {
    }
    
    void paint(Graphics& g) override
    {
        jassert(mData != nullptr);
        
        auto b = getLocalBounds();
        b.reduce(5, 5);
        
        g.setColour(Colour::greyLevel(0.2f));
        g.drawRoundedRectangle(b.toFloat(), 5.0f, 1.0f);
        
        const int margin = 10;
        b.reduce(margin, 0);
                
        g.setColour(Colour::greyLevel(0.8f));
        g.drawText(mData->mName, b, Justification(Justification::Flags::centredLeft), true);
        
        g.setColour(Colour::greyLevel(0.6f));
        g.drawText(mData->mOSCAddress, b, Justification(Justification::Flags::centredRight), true);
    }
    
    void setData(const SoundFileData* newData) { mData = newData; }
    
private:

    const SoundFileData* mData;

};

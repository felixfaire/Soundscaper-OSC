/*
  ==============================================================================

    ViewAxesIndicator.h
    Created: 27 Mar 2020 2:02:55pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Utils/ViewAxes.h"

class ViewAxesIndicator : public Component
{
public:
    ViewAxesIndicator()
    {
    }
    
    void paint(Graphics& g) override
    {
        const float size = 15.0f;
        auto hori = getLocalBounds().removeFromBottom(size);
        auto vert = getLocalBounds().removeFromLeft(size);
        
        auto names = Axes::getAxesNames(mCurrentAxes);
        
        g.setColour(Colour::greyLevel(0.8f));
        g.drawText(names.first, hori.removeFromRight(size), Justification(Justification::Flags::centred), false);
        g.drawText(names.second, vert.removeFromTop(size), Justification(Justification::Flags::centred), false);
        
        const float ptSize = 3;
        hori.reduce(ptSize, ptSize);
        vert.reduce(ptSize, ptSize);
        
        g.setColour(Colour::greyLevel(0.4f));
        g.drawHorizontalLine(hori.getCentreY(), hori.getX(), hori.getRight());
        g.drawVerticalLine(  vert.getCentreX(), vert.getY(), vert.getBottom());
    
        auto r = Rectangle<float>().withSize((float)ptSize, (float)ptSize);
        g.fillEllipse(r.withCentre({ (float)hori.getX(), (float)hori.getCentreY() + 0.5f }));
        g.fillEllipse(r.withCentre({ (float)hori.getRight(), (float)hori.getCentreY() + 0.5f }));
        g.fillEllipse(r.withCentre({ (float)vert.getCentreX() + 0.5f, (float)vert.getY() }));
        g.fillEllipse(r.withCentre({ (float)vert.getCentreX() + 0.5f, (float)vert.getBottom() }));
    }
    
    void setViewAxes(ViewAxes newAxes)
    {
        mCurrentAxes = newAxes;
        repaint();
    }

private:

    ViewAxes mCurrentAxes;

};

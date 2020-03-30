/*
  ==============================================================================

    SpaceViewerSilhouette.h
    Created: 27 Mar 2020 3:49:39pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Utils/ViewAxes.h"
#include "IconPaths.h"

class SpaceViewerSilhouette
{
public:
    SpaceViewerSilhouette()
    {
        mTop  = IconPaths::getTopPath();
        mSide = IconPaths::getSideOnPath();
        mBack = IconPaths::getBackPath();
    }
    
    void paint(Point<float> p, Graphics& g, ViewAxes axes, float scale)
    {
        g.setColour(Colours::white.withAlpha(0.1f));
        
        AffineTransform t = AffineTransform().scaled(1.75f * scale).translated(p.x, p.y);
        
        if (axes == ViewAxes::XZ) // Top
        {
            g.strokePath(mTop, PathStrokeType(1.5f), t);
        }
        else if (axes == ViewAxes::XY) // Back
        {
            g.fillPath(mBack, t);
        }
        else if (axes == ViewAxes::ZY) // Side
        {
            g.fillPath(mSide, t);
        }
    }
    
private:

    Path mTop;
    Path mSide;
    Path mBack;

};

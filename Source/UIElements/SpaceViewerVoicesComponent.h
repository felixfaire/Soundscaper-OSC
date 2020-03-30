/*
  ==============================================================================

    SpaceViewerVoiceVisualiserComponent.h
    Created: 30 Mar 2020 6:05:58pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../State/VisualPlayingVoicesState.h"

class SpaceViewerVoicesComponent : public Component,
                                   public ChangeListener
{
public:
    SpaceViewerVoicesComponent(VisualPlayingVoicesState& state, WorldViewState& worldView)
        : mVoicesState(state),
          mViewState(worldView)
    {
        mVoicesState.addChangeListener(this);

        setInterceptsMouseClicks(false, false);
    }

    ~SpaceViewerVoicesComponent()
    {
        mVoicesState.removeChangeListener(this);
    }

    void paint(Graphics& g) override
    {
        g.setColour(Colour::greyLevel(0.6f));
        
        const auto currTime = Time::getMillisecondCounter();

        for (const auto& v : mVoicesState.mPlayingVoices)
        {
            const auto p = mViewState.getWorldToRect(v.mPosition);
            const float t = (float)v.getProgress(currTime);
            
            drawCircle(g, p.x, p.y, 10.0f * (1.0f - t));
        }
    }

private:

    void drawCircle(Graphics& g, float x, float y, float r)
    {
        g.fillEllipse(x - r, y - r, r * 2.0f, r * 2.0f);
    }

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        repaint();
    }

    VisualPlayingVoicesState& mVoicesState;
    WorldViewState&           mViewState;

};

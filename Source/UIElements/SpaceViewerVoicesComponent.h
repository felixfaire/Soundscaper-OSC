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
        g.setColour(Colours::white);

        for (const auto& v : mVoicesState.mPlayingVoices)
        {
            const auto p = mViewState.getWorldToRect(v.mPosition);
            drawCircle(g, p.x, p.y, 5.0f, 1.0f);
        }
    }

private:

    void drawCircle(Graphics& g, float x, float y, float r, float thickness)
    {
        g.drawEllipse(x - r, y - r, r * 2.0f, r * 2.0f, thickness);
    }

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        repaint();
    }

    VisualPlayingVoicesState& mVoicesState;
    WorldViewState&           mViewState;

};

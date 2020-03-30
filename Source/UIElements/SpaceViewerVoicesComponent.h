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
    SpaceViewerVoicesComponent(VisualPlayingVoicesState& state)
        : mVoicesState(state)
    {
        mVoicesState.addChangeListener(this);
    }

    ~SpaceViewerVoicesComponent()
    {
        mVoicesState.removeChangeListener(this);
    }

    void paint(Graphics& g) override
    {

    }

private:

    void changeListenerCallback(ChangeBroadcaster* source)
    {
        repaint();
    }

    VisualPlayingVoicesState& mVoicesState;

};
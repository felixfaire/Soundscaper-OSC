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


        for (const auto& v : mVoicesState.mPlayingVoices)
        {
            const auto p = mViewState.getWorldToRect(v.mPosition);
            const float t = v.mProgress;
            
            drawCircle(g, p.x, p.y, 10.0f * (1.0f - t), 2.0f);
            drawSolidCircle(g, p.x, p.y, 4.0f * (1.0f - t));

            if (v.hasMoved())
            {
                float thickness = 0.2f;
                float step = 0.8f / (float)v.mPositionHistory.size();

                Path path;

                for (int i = 0; i < v.mPositionHistory.size(); ++i)
                {
                    const auto hp2D = mViewState.getWorldToRect(v.mPositionHistory[i]);
                    //drawCircle(g, hp2D.x, hp2D.y, 5.0f * thickness * (1.0f - t));
                    //thickness += step;

                    if (i == 0)
                        path.startNewSubPath(hp2D.x, hp2D.y);
                    else
                        path.lineTo(hp2D.x, hp2D.y);
                }

                g.strokePath(path, PathStrokeType(2.0f * (1.0f - t)));
            }
        }
    }

private:

    void drawCircle(Graphics& g, float x, float y, float r, float thickness)
    {
        g.drawEllipse(x - r, y - r, r * 2.0f, r * 2.0f, thickness);
    }

    void drawSolidCircle(Graphics& g, float x, float y, float r)
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

/*
  ==============================================================================

    SpaceViewerComponent.h
    Created: 13 Feb 2020 8:19:31pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vec2.hpp"

#include "../State/WorldViewState.h"

#include "../Utils/ConvexHullPath.h"
#include "SpeakerHandleComponent.h"
#include "ViewAxesButtons.h"
#include "ViewAxesIndicator.h"
#include "SpaceViewerSilhouette.h"
#include "SpaceViewerVoicesComponent.h"

//==============================================================================
/* This component visualises the spatial layout of the speakers.
*/
class SpaceViewerComponent    : public Component
{
public:
    
    SpaceViewerComponent(SpeakerPositionsState& speakersState, VisualPlayingVoicesState& voiceState)
        : mSpeakersState(speakersState),
          mVisualVoices(voiceState, mViewState)
    {
        mViewAxesButtons.onViewAxesChanged = [this](ViewAxes v) {
            setViewAxes(v);
        };
        
        addAndMakeVisible(mVisualVoices);
        addAndMakeVisible(mViewAxesButtons);
        addAndMakeVisible(mAxesIndicator);
        
        mAxesIndicator.setViewAxes(mViewState.mCurrentAxes);

        setBufferedToImage(true);
    }

    ~SpaceViewerComponent()
    {
    }

    void paint (Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        
        g.setColour(Colour::greyLevel(0.05f));
        g.fillRoundedRectangle(b, 4.0f);
        
        g.setColour(Colour::greyLevel(0.2f));
        g.drawRoundedRectangle(b, 4.0f, 1.0f);
        
        const float minSize = jmin(b.getWidth(), b.getHeight());
        const float meter = minSize / mViewState.mWindowDiameter;
        const float baselineProportion = mViewState.getHeightBaselineProportion();
        const int middleX = (int)b.getCentreX();
        const int middleY = (int)b.getHeight() * baselineProportion;
        
        // Draw Grid
        g.setColour(Colour::greyLevel(0.2f));
        g.drawVerticalLine(middleX,   b.getY(), b.getBottom());
        g.drawHorizontalLine(middleY, b.getX(), b.getRight());
        
        g.setColour(Colour::greyLevel(0.1f));
        for (auto x = meter; x < b.getWidth(); x += meter)
        {
            g.drawVerticalLine(middleX + x, b.getY(), b.getBottom());
            g.drawVerticalLine(middleX - x, b.getY(), b.getBottom());
        }

        for (auto y = meter; y < b.getHeight() * baselineProportion; y += meter)
        {
            g.drawHorizontalLine(middleY + y, b.getX(), b.getRight());
            g.drawHorizontalLine(middleY - y, b.getX(), b.getRight());
        }
        
        // Draw silhouette
        const float silhouetteScale = meter / 100.0f;
        mSilhouette.paint({ (float)middleX, (float)middleY }, g, mViewState.mCurrentAxes, silhouetteScale);
        
        // Draw convex hull
        g.setColour(Colour(10, 50, 100).withAlpha(0.1f));
        g.fillPath(mHullPath.mPath);
        
        g.setColour(Colours::white.withAlpha(0.1f));
        g.strokePath(mHullPath.mPath, PathStrokeType(2.0f));
    }

    void resized() override
    {
        mVisualVoices.setBounds(getLocalBounds());
        const int inset = 10;
        
        mViewAxesButtons.setBounds(getLocalBounds().reduced(inset).removeFromBottom(35).removeFromRight(180));
        
        const int axesSize = 100;
        mAxesIndicator.setBounds(getLocalBounds().reduced(inset).removeFromBottom(axesSize).removeFromLeft(axesSize));

        updateSpeakerButtonComponents();
    }
    
    void mouseDown(const MouseEvent& event) override
    {
        const auto pos = mViewState.getRectToWorld(glm::vec2(event.getPosition().x, event.getPosition().y));
        
        if (onTrigger != nullptr)
            onTrigger(pos);
    }
    
    void mouseDrag(const MouseEvent& event) override
    {
        const auto pos = mViewState.getRectToWorld(glm::vec2(event.getPosition().x, event.getPosition().y));

        if (onUpdate != nullptr)
            onUpdate(pos);
    }
        
    void updateComponentPositions()
    {
        if (mSpeakersState.getPositions().size() != mSpeakerHandles.size())
            createSpeakerComponents();
        
        mViewState.updateZoomExtents(getLocalBounds().toFloat(), mSpeakersState.getPositions());
        updateSpeakerButtonComponents();
    }
    
    void setViewAxes(ViewAxes newViewAxes)
    {
        mViewState.mCurrentAxes = newViewAxes;
        mAxesIndicator.setViewAxes(newViewAxes);
        updateComponentPositions();
    }
    
    std::function<void(glm::vec3)> onTrigger;
    std::function<void(glm::vec3)> onUpdate;

private:

    void createSpeakerComponents()
    {
        mSpeakerHandles.clear();
        
        auto onUpdatePosition = [this](int i, const glm::vec3& p) {
            mSpeakersState.setSpeakerPosition(i, p);
        };
        
        auto onHandleDragged = [this](int i, const glm::vec2& drag)
        {
            const auto diff = mViewState.getRectToWorld(drag, false);
            const auto newPos = mSpeakersState.getPosition(i) + diff;
            mSpeakersState.setSpeakerPosition(i, newPos);
        };
        
        auto onHandleReleased = [this](int i)
        {
            auto p = mSpeakersState.getPosition(i);
            p = glm::round(p * 10.0f) * 0.1f;
            mSpeakersState.setSpeakerPosition(i, p);
        };

        for (int i = 0; i < mSpeakersState.getPositions().size(); ++i)
        {
            SpeakerHandleComponent* c = new SpeakerHandleComponent(mSpeakersState, i);
            c->onUpdatePosition = onUpdatePosition;
            c->onDrag = onHandleDragged;
            c->onHandleReleased = onHandleReleased;
            const int size = 25 + (int)(5.0f + getDepthNormalized(mSpeakersState.getPosition(i)));
            c->setSize(size, size);
            addAndMakeVisible(*c);
            mSpeakerHandles.add(c);
        }
    }

    void updateSpeakerButtonComponents()
    {
        if (mSpeakerHandles.size() == 0)
            return;

        mViewState.updateZoomExtents(getLocalBounds().toFloat(), mSpeakersState.getPositions());
            
        std::vector<glm::vec2> uiPositions(mSpeakerHandles.size());

        // Position speaker components
        for (int i = 0; i < mSpeakerHandles.size(); ++i)
        {
            const auto& s = mSpeakersState.getPosition(i);
            const auto p = mViewState.getWorldToRect(s);
            const int size = 25 - (int)(5.0f * getDepthNormalized(s));
            mSpeakerHandles[i]->setSize(size, size);
            mSpeakerHandles[i]->setPosition(p.x, p.y);
            uiPositions[i] = p;
        }

        mHullPath.updatePoints(uiPositions);
        repaint();
    }
    
    float getDepthNormalized(const glm::vec3& p)
    {
        const float v = Axes::getDepthValue(mViewState.mCurrentAxes, p);
        return glm::clamp(v / 4.0f, -1.0f, 1.0f);
    }
    
    
    // State
    SpeakerPositionsState&  mSpeakersState;
    WorldViewState          mViewState;

    // Drawing
    ConvexHullPath          mHullPath;
    SpaceViewerSilhouette   mSilhouette;
    
    // Components
    SpaceViewerVoicesComponent          mVisualVoices;
    OwnedArray<SpeakerHandleComponent>  mSpeakerHandles;
    ViewAxesButtons                     mViewAxesButtons;
    ViewAxesIndicator                   mAxesIndicator;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpaceViewerComponent)
};

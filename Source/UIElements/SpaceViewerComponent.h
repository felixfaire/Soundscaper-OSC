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
#include "gtx/matrix_transform_2d.hpp"
#include "../Utils/ConvexHullPath.h"
#include "SpeakerHandleComponent.h"
#include "ViewAxesButtons.h"
#include "ViewAxesIndicator.h"
#include "SpaceViewerSilhouette.h"

//==============================================================================
/* This component visualises the spatial layout of the speakers.
*/
class SpaceViewerComponent    : public Component
{
public:
    
    SpaceViewerComponent(SpeakerPositionsState& speakersState)
        : mSpeakersState(speakersState)
    {
        mViewAxesButtons.onViewAxesChanged = [this](ViewAxes v) {
            setViewAxes(v);
        };
        
        addAndMakeVisible(mViewAxesButtons);
        addAndMakeVisible(mAxesIndicator);
        
        mAxesIndicator.setViewAxes(mCurrentViewAxes);
        
        updateZoomExtents();
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
        const float meter = minSize / mWindowDiameter;
        const int middleX = (int)b.getCentreX();
        const int middleY = (int)b.getHeight() * getHeightBaselineProportion();
        
        // Draw Grid
        g.setColour(Colour::greyLevel(0.2f));
        g.drawVerticalLine(middleX,   b.getY(), b.getBottom());
        g.drawHorizontalLine(middleY, b.getX(), b.getRight());
        
        g.setColour(Colour::greyLevel(0.1f));
        for (auto x = meter; x < b.getWidth() * getHeightBaselineProportion(); x += meter)
        {
            g.drawVerticalLine(middleX + x, b.getY(), b.getBottom());
            g.drawVerticalLine(middleX - x, b.getY(), b.getBottom());
        }

        for (auto y = meter; y < b.getHeight() * getHeightBaselineProportion(); y += meter)
        {
            g.drawHorizontalLine(middleY + y, b.getX(), b.getRight());
            g.drawHorizontalLine(middleY - y, b.getX(), b.getRight());
        }
        
        // Draw silhouette
        const float silhouetteScale = meter / 100.0f;
        mSilhouette.paint({ (float)middleX, (float)middleY }, g, mCurrentViewAxes, silhouetteScale);
        
        // Draw convex hull
        g.setColour(Colour(10, 50, 100).withAlpha(0.1f));
        g.fillPath(mHullPath.mPath);
        
        g.setColour(Colours::white.withAlpha(0.1f));
        g.strokePath(mHullPath.mPath, PathStrokeType(2.0f));
    }

    void resized() override
    {
        const int inset = 10;
        
        mViewAxesButtons.setBounds(getLocalBounds().reduced(inset).removeFromBottom(35).removeFromRight(180));
        
        const int axesSize = 100;
        mAxesIndicator.setBounds(getLocalBounds().reduced(inset).removeFromBottom(axesSize).removeFromLeft(axesSize));
        
        updateMatrices();
        updateSpeakerButtonComponents();
    }
    
    void mouseDown(const MouseEvent& event) override
    {
        const auto pos = getWorldPositionFromMouse(event);
        
        if (onTrigger != nullptr)
            onTrigger(pos);
    }
    
    void mouseDrag(const MouseEvent& event) override
    {
        const auto pos = getWorldPositionFromMouse(event);

        if (onUpdate != nullptr)
            onUpdate(pos);
    }
        
    void updateComponentPositions()
    {
        if (mSpeakersState.getSpeakerPositions().size() != mSpeakerHandles.size())
            createSpeakerComponents();
        
        updateZoomExtents();
        updateSpeakerButtonComponents();
    }
    
    void setViewAxes(ViewAxes newViewAxes)
    {
        mCurrentViewAxes = newViewAxes;
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
            const auto diff = getRectToWorld(drag, false);
            const auto newPos = mSpeakersState.getSpeakerPosition(i) + Axes::getUnflattenedPoint(mCurrentViewAxes, diff);
            mSpeakersState.setSpeakerPosition(i, newPos);
        };
        
        auto onHandleReleased = [this](int i)
        {
            auto p = mSpeakersState.getSpeakerPosition(i);
            p = glm::round(p * 10.0f) * 0.1f;
            mSpeakersState.setSpeakerPosition(i, p);
        };

        for (int i = 0; i < mSpeakersState.getSpeakerPositions().size(); ++i)
        {
            SpeakerHandleComponent* c = new SpeakerHandleComponent(mSpeakersState, i);
            c->onUpdatePosition = onUpdatePosition;
            c->onDrag = onHandleDragged;
            c->onHandleReleased = onHandleReleased;
            const int size = 25 + (int)(5.0f + getDepthNormalized(mSpeakersState.getSpeakerPosition(i)));
            c->setSize(size, size);
            addAndMakeVisible(*c);
            mSpeakerHandles.add(c);
        }
    }

    void updateZoomExtents()
    {
        mWindowDiameter = 1.0f;
        
        // TODO: fix assumption that lengths are calculated from the center
        
        for (const auto& p : mSpeakersState.getSpeakerPositions())
        {
            const float l = glm::length(Axes::getFlattenedPoint(mCurrentViewAxes, p));
            
            if (l > mWindowDiameter)
                mWindowDiameter = l;
        }
        
        mWindowDiameter *= 2.2f;
        mMinWindowDiameter = mWindowDiameter;
        updateMatrices();
    }

    void updateSpeakerButtonComponents()
    {
        if (mSpeakerHandles.size() == 0)
            return;
            
        std::vector<glm::vec2> uiPositions(mSpeakerHandles.size());

        // Position speaker components
        for (int i = 0; i < mSpeakerHandles.size(); ++i)
        {
            const auto& s = mSpeakersState.getSpeakerPosition(i);
            const auto p = getWorldToRect(Axes::getFlattenedPoint(mCurrentViewAxes, s));
            const int size = 25 - (int)(5.0f * getDepthNormalized(s));
            mSpeakerHandles[i]->setSize(size, size);
            mSpeakerHandles[i]->setPosition(p.x, p.y);
            uiPositions[i] = p;
        }

        mHullPath.updatePoints(uiPositions);
        updateZoomExtents();
        repaint();
    }

    const glm::vec3 getWorldPositionFromMouse(const MouseEvent& event)
    {
        const auto mp = event.getPosition();
        const auto p = getRectToWorld(glm::vec2(mp.x, mp.y));
        return glm::vec3(p.x, 0.0f, p.y);
    }
    
    void drawCircle(Graphics& g, float x, float y, float r, float thickness)
    {
        g.drawEllipse(x - r, y - r, r * 2.0f, r * 2.0f, thickness);
    }
    
    glm::vec2 getRectToWorld(const glm::vec2& p, bool translate = true)
    {
        const auto pt = mRectToWorld * glm::vec3(p.x, p.y, translate ? 1.0f : 0.0f);
        return glm::vec2(pt.x, pt.y);
    }
    
    glm::vec2 getWorldToRect(const glm::vec2& p)
    {
        const auto pt = mWorldToRect * glm::vec3(p.x, p.y, 1.0f);
        return glm::vec2(pt.x, pt.y);
    }
    
    void updateMatrices()
    {
        auto b = getLocalBounds();
        const float minDim = jmin(b.getWidth(), b.getHeight());
        
        glm::mat3 t = glm::mat3(1.0f);
        t = glm::scale(t, glm::vec2(1.0f) * mWindowDiameter);
        t = glm::scale(t, glm::vec2(1.0f / minDim));
        t = glm::scale(t, glm::vec2(1.0f, -1.0f)); // flip y up for world down for window rect
        t = glm::translate(t, glm::vec2(-b.getCentreX(), -getHeight() * getHeightBaselineProportion()));
        
        mRectToWorld = t;
        mWorldToRect = glm::inverse(t);
    }
    
    float getHeightBaselineProportion()
    {
        if (mCurrentViewAxes == ViewAxes::XZ)
            return 0.5f;
        else
            return 0.75f;
    }
    
    float getDepthNormalized(const glm::vec3& p)
    {
        const float v = Axes::getDepthValue(mCurrentViewAxes, p);
        return glm::clamp(v / 4.0f, -1.0f, 1.0f);
    }
    
    
    ViewAxes      mCurrentViewAxes = ViewAxes::XZ;

    glm::mat3 mRectToWorld;
    glm::mat3 mWorldToRect;

    SpeakerPositionsState& mSpeakersState;
    
    ConvexHullPath mHullPath;
    
    float     mWindowDiameter = 4.0f;
    float     mMinWindowDiameter = 4.0f;
    float     mMaxWindowDiameter = 100.0f;

    
    OwnedArray<SpeakerHandleComponent> mSpeakerHandles;

    ViewAxesButtons     mViewAxesButtons;
    ViewAxesIndicator   mAxesIndicator;
    
    SpaceViewerSilhouette mSilhouette;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpaceViewerComponent)
};

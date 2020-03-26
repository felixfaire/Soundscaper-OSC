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
#include "Utils/ConvexHullPath.h"
#include "UIElements/SpeakerHandleComponent.h"


//==============================================================================
/*
*/
class SpaceViewerComponent    : public Component
{
public:
    SpaceViewerComponent(AppModel& model)
        : mModel(model)
    {
        mAddButton.reset(new TextButton("Add"));
        mRemoveButton.reset(new TextButton("Remove"));
        
        mAddButton->onClick = [this] () {
            auto& r = Random::getSystemRandom();
            mModel.addSpeaker(glm::vec3(r.nextFloat(), r.nextFloat(), r.nextFloat()) * 2.0f - 1.0f);
        };
        
        mRemoveButton->onClick = [this] () {
            mModel.removeSpeaker();
        };
        
        addAndMakeVisible(*mAddButton);
        addAndMakeVisible(*mRemoveButton);
        
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
        const int middleY = (int)b.getCentreY();
        
        // Draw Grid
        g.setColour(Colour::greyLevel(0.2f));
        g.drawVerticalLine(middleX,   b.getY(), b.getBottom());
        g.drawHorizontalLine(middleY, b.getX(), b.getRight());
        
        for (auto x = meter; x < b.getWidth() * 0.5f; x += meter)
        {
            g.drawVerticalLine(middleX + x, b.getY(), b.getBottom());
            g.drawVerticalLine(middleX - x, b.getY(), b.getBottom());
        }

        for (auto y = meter; y < b.getHeight() * 0.5f; y += meter)
        {
            g.drawHorizontalLine(middleY + y, b.getX(), b.getRight());
            g.drawHorizontalLine(middleY - y, b.getX(), b.getRight());
        }
        
        // Draw convex hull
        g.setColour(Colours::white.withAlpha(0.3f));
        g.strokePath(mHullPath.mPath, PathStrokeType(2.0f));
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced(10);
        b = b.removeFromBottom(50).reduced(10);
        mRemoveButton->setBounds(b.removeFromRight(mRemoveButton->getBestWidthForHeight(b.getHeight())));
        b.removeFromRight(10);
        mAddButton->setBounds(b.removeFromRight(mAddButton->getBestWidthForHeight(b.getHeight())));
        
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
        if (mModel.getSpeakerPositions().size() != mSpeakers.size())
            createSpeakerComponents();
        
        updateZoomExtents();
        updateSpeakerButtonComponents();
    }
    
    std::function<void(glm::vec3)> onTrigger;
    std::function<void(glm::vec3)> onUpdate;

private:

    void createSpeakerComponents()
    {
        mSpeakers.clear();
        
        auto onUpdatePosition = [this](int i, const glm::vec3& p) {
            mModel.setSpeakerPosition(i, p);
        };
        
        auto onHandleDragged = [this](int i, const glm::vec2& drag)
        {
            const auto diff = getRectToWorld(drag, false);
            const auto newPos = mModel.getSpeakerPosition(i) + glm::vec3(diff.x, 0.0f, diff.y);
            mModel.setSpeakerPosition(i, newPos);
        };
        
        auto onHandleReleased = [this](int i)
        {
            auto p = mModel.getSpeakerPosition(i);
            p = glm::round(p);
            mModel.setSpeakerPosition(i, p);
        };

        for (int i = 0; i < mModel.getSpeakerPositions().size(); ++i)
        {
            SpeakerHandleComponent* c = new SpeakerHandleComponent(mModel, i);
            c->onUpdatePosition = onUpdatePosition;
            c->onDrag = onHandleDragged;
            c->onHandleReleased = onHandleReleased;
            c->setSize(25, 25);
            addAndMakeVisible(*c);
            mSpeakers.add(c);
        }
    }

    void updateZoomExtents()
    {
        mWindowDiameter = 1.0f;
        
        for (const auto& p : mModel.getSpeakerPositions())
        {
            const float l = glm::length(glm::vec2(p.x, p.z));
            
            if (l > mWindowDiameter)
                mWindowDiameter = l;
        }
        
        mWindowDiameter *= 2.2f;
        mMinWindowDiameter = mWindowDiameter;
        updateMatrices();
    }

    void updateSpeakerButtonComponents()
    {
        if (mSpeakers.size() == 0 || getWidth() == 0 || getHeight() == 0)
            return;
            
        std::vector<glm::vec2> uiPositions(mSpeakers.size());

        // Position speaker components
        for (int i = 0; i < mSpeakers.size(); ++i)
        {
            const auto& s = mModel.getSpeakerPosition(i);
            const auto p = getWorldToRect(glm::vec2(s.x, s.z));
            mSpeakers[i]->setPosition(p.x, p.y);
            uiPositions[i] = p;
        }
        
        auto center = glm::vec2((float)getWidth(), (float)getHeight()) * 0.5f;
        mHullPath.updatePoints(uiPositions, center);
        
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
        t = glm::translate(t, glm::vec2(-b.getCentreX(), -b.getCentreY()));
        
        mRectToWorld = t;
        mWorldToRect = glm::inverse(t);
    }

    glm::mat3 mRectToWorld;
    glm::mat3 mWorldToRect;

    AppModel& mModel;
    
    ConvexHullPath mHullPath;
    
    float     mWindowDiameter = 4.0f;
    float     mMinWindowDiameter = 4.0f;
    float     mMaxWindowDiameter = 100.0f;

    OwnedArray<SpeakerHandleComponent> mSpeakers;
    
    std::unique_ptr<TextButton> mAddButton;
    std::unique_ptr<TextButton> mRemoveButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpaceViewerComponent)
};

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
        addAndMakeVisible(mDemoFileBox);
        
        auto onUpdatePosition = [this](int i, const glm::vec3& p) {
            mModel.mSpeakerPositions[i] = p;
            updateSpeakerBounds();
            updateSpeakerButtonComponents();
        };
        
        auto onHandleDragged = [this](int i, const glm::vec2& drag)
        {
            const auto diff = getRectToWorld(drag, false);
            mModel.mSpeakerPositions[i].x += diff.x;
            mModel.mSpeakerPositions[i].z += diff.y;
            updateSpeakerBounds();
            updateSpeakerButtonComponents();
        };

        for (int i = 0; i < mModel.mSpeakerPositions.size(); ++i)
        {
            SpeakerHandleComponent* c = new SpeakerHandleComponent(mModel, i);
            c->onUpdatePosition = onUpdatePosition;
            c->onDrag = onHandleDragged;
            c->setSize(20, 20);
            addAndMakeVisible(*c);
            mSpeakers.add(c);
        }
        
        updateSpeakerBounds();
    }

    ~SpaceViewerComponent()
    {
    }

    void paint (Graphics& g) override
    {
        auto b = getViewerRect();
        
        g.setColour(Colour::greyLevel(0.1f));
        g.fillRoundedRectangle(b, 4.0f);
        
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
        mDemoFileBox.setBounds(b.removeFromTop(mBoxHeight));
        //updateSpeakerBounds();
        updateMatrices();
        updateSpeakerButtonComponents();
    }
    
    void mouseDown(const MouseEvent& event) override
    {
        const int index = (int)mDemoFileBox.getSelectedId() - 1;
        const auto pos = getWorldPositionFromMouse(event);
        
        jassert(index < mModel.mSoundFiles.size());
        
        if (onTrigger != nullptr)
            onTrigger(index, pos);
    }
    
    void mouseDrag(const MouseEvent& event) override
    {
        const int index = (int)mDemoFileBox.getSelectedId() - 1;
        const auto pos = getWorldPositionFromMouse(event);
        
        jassert(index < mModel.mSoundFiles.size());
        
        if (onUpdate != nullptr)
            onUpdate(index, pos);
    }
    
    void updateFileList()
    {
        mDemoFileBox.clear();
        
        for (int i = 0; i < mModel.mSoundFiles.size(); ++i)
            mDemoFileBox.addItem(mModel.mSoundFiles[i].getFileNameWithoutExtension(), i + 1);
        
        if (mModel.mSoundFiles.size() > 0)
            mDemoFileBox.setSelectedId(1);
    }
    
    void updateSpeakerBounds()
    {
        mWindowDiameter = 1.0f;
        
        for (const auto& p : mModel.mSpeakerPositions)
        {
            const float l = glm::length(glm::vec2(p.x, p.z));
            
            if (l > mWindowDiameter)
                mWindowDiameter = l;
        }
        
        mWindowDiameter *= 2.2f;
        mMinWindowDiameter = mWindowDiameter;
    }
    
    void mouseMagnify (const MouseEvent& event, float scaleFactor) override
    {
        mWindowDiameter = jmin(mMaxWindowDiameter, jmax(mMinWindowDiameter, mWindowDiameter / scaleFactor));
        resized();
        repaint();
    }
    
    std::function<void(int, glm::vec3)> onTrigger;
    std::function<void(int, glm::vec3)> onUpdate;

private:

    void updateSpeakerButtonComponents()
    {
        std::vector<glm::vec2> uiPositions(mSpeakers.size());

        // Position speaker components
        for (int i = 0; i < mSpeakers.size(); ++i)
        {
            const auto& s = mModel.mSpeakerPositions[i];
            const auto p = getWorldToRect(glm::vec2(s.x, s.z));
            mSpeakers[i]->setPosition(p.x, p.y);
            uiPositions[i] = p;
        }
        
        auto center = glm::vec2((float)getWidth(), (float)getHeight()) * 0.5f;
        mHullPath.updatePoints(uiPositions, center);
        repaint();
    }

    Rectangle<float> getViewerRect()
    {
        auto b = getLocalBounds().reduced(10).toFloat();
        b.removeFromTop(mBoxHeight);
        b.removeFromTop(10.0f);
        return b;
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
        auto b = getViewerRect();
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
    ComboBox  mDemoFileBox;
    int       mBoxHeight = 40;
    AppModel& mModel;
    
    ConvexHullPath mHullPath;
    
    float     mWindowDiameter = 4.0f;
    float     mMinWindowDiameter = 4.0f;
    float     mMaxWindowDiameter = 100.0f;

    OwnedArray<SpeakerHandleComponent> mSpeakers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpaceViewerComponent)
};

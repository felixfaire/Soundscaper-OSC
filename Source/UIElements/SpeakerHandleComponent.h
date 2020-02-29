/*
  ==============================================================================

    SpeakerHandleComponent.h
    Created: 26 Feb 2020 10:30:47pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vec2.hpp"
#include "SpeakerEditorPopupComponent.h"

class SpeakerHandleComponent : public Button,
                               public ChangeListener
{
public:
    SpeakerHandleComponent(AppModel& m, int id)
        : Button("speaker_" + std::to_string(id)),
          mModel(m),
          mID(id)
    {
        setRepaintsOnMouseActivity(true);
    }

    void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override
    {
        auto b = getLocalBounds().reduced(2.0f).toFloat();
        b += Point<float>(mSubPixelDiff.x, mSubPixelDiff.y);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour(Colours::white.withAlpha(0.2f));
            g.fillEllipse(b);
        }

        g.setColour(Colours::white.withAlpha(0.8f));
        g.drawEllipse(b, 2.0f);
    }
    
    void mouseDown(const MouseEvent& event) override
    {
        mLastDrag = glm::vec2((float)event.getScreenX(), (float)event.getScreenY());
    }
    
    void mouseDrag(const MouseEvent& event) override
    {
        auto p = glm::vec2((float)event.getScreenX(), (float)event.getScreenY());
        auto diff = p - mLastDrag;
                
        if (onDrag != nullptr && glm::length(diff) != 0)
            onDrag(mID, diff);
        
        mLastDrag = p;
    }
    

    
    void mouseUp(const MouseEvent& event) override
    {
        if (onHandleReleased != nullptr && event.mouseWasDraggedSinceMouseDown())
            onHandleReleased(mID);
        
        if (!event.mouseWasClicked())
            return;
            
        auto* speakerEditor = new SpeakerEditorPopupComponent(mID, mModel.mSpeakerPositions[mID]);
        speakerEditor->addChangeListener(this);
        speakerEditor->setSize (250, 120);

        auto& cb = CallOutBox::launchAsynchronously(speakerEditor, getBounds(), getParentComponent());
        cb.setDismissalMouseClicksAreAlwaysConsumed(true);
    }
    
    void setPosition(float x, float y)
    {
        glm::vec2 p(x, y);
        glm::vec2 pr(roundToInt(x), roundToInt(y));
        setCentrePosition((int)pr.x, (int)pr.y);
        mPosition = p;
        mSubPixelDiff = p - pr;
    }

    const glm::vec2& getPosition() { return mPosition; }
    
    std::function<void(int, const glm::vec3&)> onUpdatePosition;
    std::function<void(int, const glm::vec2&)> onDrag;
    std::function<void(int)>                   onHandleReleased;
    
private:

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (auto* se = dynamic_cast<SpeakerEditorPopupComponent*>(source))
            if (onUpdatePosition != nullptr)
                onUpdatePosition(mID, se->getPosition());
    }
    
    AppModel& mModel;
    int mID = 0;
    glm::vec2 mPosition;
    glm::vec2 mSubPixelDiff;
    glm::vec2 mLastDrag;
};

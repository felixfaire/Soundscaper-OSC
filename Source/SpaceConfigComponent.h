/*
  ==============================================================================

    SpaceConfigComponent.h
    Created: 6 Mar 2020 3:28:28pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SpaceViewerComponent.h"

class SpaceConfigComponent : public Component,
                             public ChangeListener
{
public:
    SpaceConfigComponent(AppModel& model)
        : mModel(model)
    {
        mModel.mSpeakerPositionsChanges.addChangeListener(this);
        
        if (!mSender.connect ("127.0.0.1", 9001))
            showConnectionErrorMessage ("Error: could not connect to UDP port 9001.");
        
        mSpace.reset(new SpaceViewerComponent(mModel));
        
        mSpace->onTrigger = [this](glm::vec3 p){
            
            const int noteID = ++mModel.mCurrentNoteID;
            const int soundID = mDemoFileBox.getSelectedItemIndex();
            mSender.send("/start", noteID,
                                   soundID,
                                   p.x, p.y, p.z);
        };
        
        mSpace->onUpdate = [this](glm::vec3 p){
            mSender.send("/update", mModel.mCurrentNoteID,
                                    p.x, p.y, p.z);
        };
        
        addAndMakeVisible(*mSpace);
        addAndMakeVisible(mDemoFileBox);
    }
    
    void resized()
    {
        auto b = getLocalBounds();
        
        mDemoFileBox.setBounds(b.removeFromTop(40));
        b.removeFromTop(5);
        mSpace->setBounds(b);
    }
    
    void updateFileList()
    {
        mDemoFileBox.clear();
        
        for (int i = 0; i < mModel.mSoundClipData.size(); ++i)
            mDemoFileBox.addItem(mModel.mSoundClipData[i].mName, i + 1);
        
        if (mModel.mSoundClipData.size() > 0)
            mDemoFileBox.setSelectedId(1);
    }

    std::unique_ptr<SpaceViewerComponent> mSpace;

private:

    //==============================================================================
    void showConnectionErrorMessage (const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
            "Connection error",
            messageText,
            "OK");
    }

    void changeListenerCallback (ChangeBroadcaster* source)
    {
        if (source == &mModel.mSpeakerPositionsChanges)
            mSpace->updateComponentPositions();
    }

    AppModel& mModel;
    ComboBox  mDemoFileBox;
    OSCSender mSender;
    
};

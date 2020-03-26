/*
  ==============================================================================

    SpaceConfigComponent.h
    Created: 6 Mar 2020 3:28:28pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UIElements/SpaceViewerComponent.h"
#include "UIElements/ChannelInfoComponent.h"
#include "UIElements/SpeakerInfoListComponent.h"

class ComponentContainer : public Component
{
public:
    ComponentContainer()
        : mContent(nullptr)
    {
    }

    void resized() override
    {
        if (mContent != nullptr)
        {
            mContent->setBounds(getLocalBounds());
        }
    }

    void setContent(Component* content) 
    { 
        jassert(content != nullptr);

        removeAllChildren();

        mContent = content;
        addAndMakeVisible(*mContent);
        mContent->setBounds(getLocalBounds());
    }

private:

    Component* mContent;
};

class SpaceConfigComponent : public Component,
                             public ChangeListener
{
public:
    SpaceConfigComponent(AppModel& model)
        : mModel(model),
          mListViewToggleButton("Enable List View")
    {
        mModel.mSpeakerPositionsChanges.addChangeListener(this);
        mModel.mAudioLevelChanges.addChangeListener(this);
        mModel.mDeviceManager.addChangeListener(this);
        
        if (!mSender.connect ("127.0.0.1", 9001))
            showConnectionErrorMessage ("Error: could not connect to UDP port 9001.");

        mSpace.reset(new SpaceViewerComponent(mModel));
        
        mSpace->onTrigger = [this](glm::vec3 p) {
            
            const int noteID = ++mModel.mCurrentNoteID;
            const int soundID = 0;
            mSender.send("/start", noteID,
                                   soundID,
                                   p.x, p.y, p.z);
        };
        
        mSpace->onUpdate = [this](glm::vec3 p) {
            mSender.send("/update", mModel.mCurrentNoteID,
                                    p.x, p.y, p.z);
        };

        mListViewToggleButton.onClick = [this]() {

            if (mListViewToggleButton.getToggleState())
                mViewContainer.setContent(mSpeakerListView.get());
            else
                mViewContainer.setContent(mSpace.get());

            this->resized();
        };

        mSpeakerListView.reset(new SpeakerInfoListComponent(mModel));
        mViewContainer.setContent(mSpace.get());
        
        addAndMakeVisible(mViewContainer);
        addAndMakeVisible(mListViewToggleButton);
    }
    
    void resized() override
    {
        auto b = getLocalBounds();
        
        auto topBar = b.removeFromTop(40);

        mListViewToggleButton.setBounds(topBar);

        b.removeFromTop(5);
        auto cb = b.removeFromBottom(20).toFloat();
        b.removeFromBottom(5);
        
        mViewContainer.setBounds(b);
        
        const float step = cb.getWidth() / (float)mChannelComponents.size();
        
        for (auto& c : mChannelComponents)
            c->setBounds(cb.removeFromLeft(step).toNearestInt().reduced(2));
    }
    

private:

    //==============================================================================
    void showConnectionErrorMessage (const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
            "Connection error",
            messageText,
            "OK");
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == &mModel.mSpeakerPositionsChanges)
        {
            mSpace->updateComponentPositions();
        
            if (mChannelComponents.size() != mModel.getSpeakerPositions().size())
            {
                mChannelComponents.clear();
                
                for (int i = 0; i < mModel.getSpeakerPositions().size(); ++i)
                {
                    mChannelComponents.emplace_back(new ChannelInfoComponent(i));
                    addAndMakeVisible(*mChannelComponents.back());
                }
                
                updateActivatedChannels();
                resized();
            }
        }
        else if (source == &mModel.mAudioLevelChanges)
        {
            jassert(mModel.mAudioLevels.size() <= mChannelComponents.size());
            
            const auto& setup = mModel.mDeviceManager.getAudioDeviceSetup();
            const auto& channels = setup.outputChannels;
            

            // TODO: sort out amplitudes getting sent to the right channel indices

//            std::vector<int> indices;
            int index = 0;
            
//            while(true)
//            {
//                index = channels.findNextSetBit(index);
//
//                if (index != -1)
//                    indices.push_back(index);
//                else
//                    return;
//            }
            
            for (const auto& l : mModel.mAudioLevels)
            {
//                index = channels.findNextSetBit(index);
                mChannelComponents[index]->setLevel(l);
                index++;
            }
        }
        else if (source == &mModel.mDeviceManager)
        {
            updateActivatedChannels();
        }
    }
    
    void updateActivatedChannels()
    {
        const auto& setup = mModel.mDeviceManager.getAudioDeviceSetup();
        const auto& channels = setup.outputChannels;
        
        for (int i = 0; i < mChannelComponents.size(); ++i)
        {
            bool channelEnabled = channels[i];
            
//            if (setup.useDefaultInputChannels)
//                channelEnabled = true;
            
            mChannelComponents[i]->setEnabled(channelEnabled);
        }
    }

    AppModel& mModel;

    std::unique_ptr<SpaceViewerComponent>      mSpace;
    std::unique_ptr<SpeakerInfoListComponent>  mSpeakerListView;

    ToggleButton    mListViewToggleButton;

    ComponentContainer mViewContainer;

    std::vector<std::unique_ptr<ChannelInfoComponent>> mChannelComponents;
    
    OSCSender mSender;
    
};

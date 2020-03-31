/*
  ==============================================================================

    ChannelMonitorComponent.h
    Created: 6 Mar 2020 3:23:12pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../State/AppModel.h"

/** A small visual indicator of a channel's current output level
    and whether it is active with the current audio device.
*/
class ChannelMonitorComponent : public Component
{
public:
    ChannelMonitorComponent(int index)
    {
        const auto sindex = std::to_string(index + 1);
        mName.reset(new Label("ch_" + sindex, sindex));
        mName->setJustificationType(Justification(Justification::Flags::centred));
        
        addAndMakeVisible(*mName);
    }
    
    void paint(Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        
        if (!isEnabled())
            mLevel = 0.0f;
        
        const float bk = isEnabled() ? 0.15f : 0.1f;
        
        g.setColour(Colour::greyLevel(bk));
        g.fillRoundedRectangle(b, 4.0f);
        
        if (isEnabled())
        {
            b.reduced(2.0f);
            b.setWidth(b.getWidth() * mLevel * 2.0f);
            g.setColour(Colours::lightgreen);
            g.fillRoundedRectangle(b, 4.0f);
        }
    }
    
    void resized() override
    {
        auto b = getLocalBounds();
        
        mName->setBounds(b);
    }
    
    void setLevel(float newLevel)
    {
        mLevel = newLevel;
        repaint();
    }
    
    
private:

    float mLevel = 0.0f;
    std::unique_ptr<Label> mName;
    
};


/** A horizontal list of all the current channels / speakers.
    Used by the SpaceConfigComponent page.
*/
class ChannelMonitorComponentBar : public Component,
                                   public ChangeListener
{
public:
    ChannelMonitorComponentBar(AppModel& model)
        : mModel(model)
    {
        mModel.mAudioMonitorState.addChangeListener(this);
        mModel.mSpeakerPositionsState.addChangeListener(this);
        mModel.mDeviceManager.addChangeListener(this);
    }
    
    ~ChannelMonitorComponentBar()
    {
        mModel.mAudioMonitorState.removeChangeListener(this);
        mModel.mSpeakerPositionsState.removeChangeListener(this);
        mModel.mDeviceManager.removeChangeListener(this);
    }

    void resized() override
    {
        auto b = getLocalBounds().toFloat();
        const float step = b.getWidth() / (float)mChannelComponents.size();
        
        for (auto& c : mChannelComponents)
            c->setBounds(b.removeFromLeft(step).toNearestInt().reduced(2));
    }
    
    void updateNumOutputChannels(const AppModel& model)
    {
        if (mChannelComponents.size() != model.mSpeakerPositionsState.getPositions().size())
        {
            mChannelComponents.clear();
            
            for (int i = 0; i < model.mSpeakerPositionsState.getPositions().size(); ++i)
            {
                mChannelComponents.emplace_back(new ChannelMonitorComponent(i));
                addAndMakeVisible(*mChannelComponents.back());
            }
            
            const auto& setup = model.mDeviceManager.getAudioDeviceSetup();
            updateActivatedChannels(setup);
            resized();
        }
    }
    
    void updateActivatedChannels(const AudioDeviceManager::AudioDeviceSetup& setup)
    {
        const auto& channels = setup.outputChannels;
        
        for (int i = 0; i < mChannelComponents.size(); ++i)
        {
            bool channelEnabled = channels[i];
            
//            if (setup.useDefaultInputChannels)
//                channelEnabled = true;
            
            mChannelComponents[i]->setEnabled(channelEnabled);
        }
    }
    
    void updateAudioLevels(const AppModel& model)
    {
        const auto& levels = model.mAudioMonitorState.getAudioLevels();

        jassert(levels.size() <= mChannelComponents.size());
            
        const auto& setup = model.mDeviceManager.getAudioDeviceSetup();
        const auto& channels = setup.outputChannels;

        int index = -1;
        
        for (const auto& l : levels)
        {
            index = channels.findNextSetBit(index + 1);
            
            if (index != -1 && index < mChannelComponents.size())
                mChannelComponents[index]->setLevel(l);
        }
    }
    
private:

    void changeListenerCallback(ChangeBroadcaster* source)
    {
        if (source == &mModel.mSpeakerPositionsState)
        {
            updateNumOutputChannels(mModel);
        }
        else if (source == &mModel.mDeviceManager)
        {
            const auto& setup = mModel.mDeviceManager.getAudioDeviceSetup();
            updateActivatedChannels(setup);
        }
        else if (source == &mModel.mAudioMonitorState)
        {
            updateAudioLevels(mModel);
        }
    }

    AppModel&                                               mModel;
    std::vector<std::unique_ptr<ChannelMonitorComponent>>   mChannelComponents;
    
};

/*
  ==============================================================================

    ClipFolderListComponent.h
    Created: 1 Apr 2020 2:28:31pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../State/AppModel.h"
#include "MinimalLookAndFeel.h"
#include "AudioDataFolderListComponent.h"

#include "NamedWaveformComponent.h"


class ClipListItemComponent : public Component
{
public:
    ClipListItemComponent(AppModel& model, int index)
        : mModel(model),
          mCurrentIndex(index)
    {
        mNamedWaveformComponent.reset(new NamedWaveformComponent());
        addAndMakeVisible(*mNamedWaveformComponent);
        
        setIndex(index);
    }

    void paint(Graphics& g) override
    {
        auto b = getLocalBounds();
        MinimalLookAndFeel::drawFileListItemBackground(g, b);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        mNamedWaveformComponent->setBounds(b.reduced(3, 7));
    }

    void setIndex(int index) 
    {
        jassert(index < mModel.mAudioDataState.mSoundClipData.size());

        mCurrentIndex = index;

        // Update audio file component
        const auto& fileList = mModel.mAudioDataState.mSoundClipData;
        const auto* fileData = &fileList[index];
        mNamedWaveformComponent->setData(fileData);
    }

private:

    AppModel&                           mModel;

    int                                     mCurrentIndex;
    std::unique_ptr<NamedWaveformComponent> mNamedWaveformComponent;

};

//==============================================================================
/*
*/
class ClipListBoxModel : public ListBoxModel
{
public:
    ClipListBoxModel(AppModel& model)
        : mModel(model)
    {
    }

    int getNumRows() override
    {
        return (int)mModel.mAudioDataState.mSoundClipData.size();
    }

    Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override
    {
        if (rowNumber >= (int)mModel.mAudioDataState.mSoundClipData.size())
            return existingComponentToUpdate;

        auto* c = static_cast<ClipListItemComponent*>(existingComponentToUpdate);

        if (c == nullptr)
        {
            c = new ClipListItemComponent(mModel, rowNumber);
        }
        else
        {
            c->setIndex(rowNumber);
        }

        return c;
    }

    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override {}

private:

    AppModel& mModel;
};


//==============================================================================
/*
*/
class ClipFolderListComponent : public AudioDataFolderListComponent
{
public:
    ClipFolderListComponent(AppModel& model, const String& title)
        : AudioDataFolderListComponent(title, model.mAudioDataState.mCurrentSoundClipFolder),
          mListBoxModel(model)
    {
        mListBox.reset(new ListBox("ClipsList", &mListBoxModel));
        mListBox->setRowHeight((int)mRowHeight);
        mListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
        addAndMakeVisible(*mListBox);
        
        mHelpButton->setTitle("Sound Clips");
        mHelpButton->setText("These sounds can be triggered at a position in space or moved over their duration. \n\nThese sounds can be triggered by their name (hover the list item to see the OSC name) or their index in this list.\n\nSee the readme for more details on OSC message formats.");
    }

private:

    ClipListBoxModel   mListBoxModel;

};

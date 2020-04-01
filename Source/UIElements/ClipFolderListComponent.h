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

#include "AudioFileComponent.h"


class ClipListItemComponent : public Component
{
public:
    ClipListItemComponent(AppModel& model, int index)
        : mModel(model),
          mCurrentIndex(index)
    {
        mAudioFileComponent.reset(new AudioFileComponent());
        addAndMakeVisible(*mAudioFileComponent);

        setIndex(index);
    }

    void resized() override
    {
        auto b = getLocalBounds();

        mAudioFileComponent->setBounds(b);
    }

    void setIndex(int index) 
    {
        jassert(index < mModel.mAudioDataState.mSoundClipData.size());

        mCurrentIndex = index;

        // Update audio file component
        const auto& fileList = mModel.mAudioDataState.mSoundClipData;
        const auto* fileData = &fileList[index];
        mAudioFileComponent->setData(fileData);
    }

private:

    AppModel&                           mModel;

    int                                 mCurrentIndex;
    std::unique_ptr<AudioFileComponent> mAudioFileComponent;

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
    }

private:

    ClipListBoxModel   mListBoxModel;

};
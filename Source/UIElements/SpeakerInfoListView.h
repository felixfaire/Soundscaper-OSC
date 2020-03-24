/*
  ==============================================================================

    SpeakerInfoListView.h
    Created: 24 Mar 2020 4:14:02pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../AppModel.h"

class SpeakerPostionComponent : public Component
{
public:
    SpeakerPostionComponent(AppModel& m, int index)
        : mModel(m),
          mIndex(index)
    {

    }


    void setIndex(int newIndex) { mIndex = newIndex; }

private:

    AppModel& mModel;
    int       mIndex;

    Label

};

class SpeakerInfoListBoxModel : public ListBoxModel
{
public:
    SpeakerInfoListBoxModel(AppModel& m)
        : mModel(m)
    {
    }

    int getNumRows() override
    {
        return (int)mModel.getSpeakerPositions().size();
    }

    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override
    {
        if (rowNumber >= mModel.getSpeakerPositions().size())
            return existingComponentToUpdate;

        const auto& position = mModel.getSpeakerPositions()[rowNumber];

        auto* c = static_cast<SpeakerPostionComponent*>(existingComponentToUpdate);

        if (c == nullptr)
            c = new SpeakerPostionComponent(mModel, rowNumber);
        else
            c->setIndex(rowNumber);

        return c;
    }

    void paintListBoxItem(int rowNumber,
        Graphics& g,
        int width, int height,
        bool rowIsSelected) override
    {
    }

    AppModel& mModel;
};


class SpeakerInfoListComponent : public Component
{
public:
    SpeakerInfoListComponent(File& filesLocation, const std::vector<SoundFileData>& fileData)
        : mFilesLocation(filesLocation),
        mListBoxModel(fileData)
    {
        mFolderChooser.reset(new FilenameComponent("Samples Folder",
            mFilesLocation,
            true, true, false, "", "",
            "Select Samples Folder"));

        mListBox.reset(new ListBox("BedsList", &mListBoxModel));
        mListBox->setRowHeight((int)mRowHeight);
        mListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);

        addAndMakeVisible(*mFolderChooser);
        addAndMakeVisible(*mListBox);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        mFolderChooser->setBounds(b.removeFromTop(50).reduced(10));
        mListBox->setBounds(b.reduced(5));
        mListBox->updateContent();
    }

    void addListener(FilenameComponentListener* listener)
    {
        mFolderChooser->addListener(listener);
    }

    void updateContent()
    {
        mListBox->updateContent();
    }

    int getIdealHeight()
    {
        return (2 + mListBoxModel.getNumRows()) * mRowHeight;
    }

    FilenameComponent* getFilenameComponent() { return mFolderChooser.get(); }

private:

    // Data
    File&                     mFilesLocation;

    // UI
    SpeakerInfoListBoxModel               mListBoxModel;
    std::unique_ptr<ListBox>            mListBox;
    std::unique_ptr<FilenameComponent>  mFolderChooser;
    const int                           mRowHeight = 50;

};
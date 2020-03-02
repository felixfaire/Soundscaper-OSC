/*
  ==============================================================================

    AudioFileListComponent.h
    Created: 12 Feb 2020 8:45:42pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UIElements/AudioFileComponent.h"

class AudioFileListBoxModel : public ListBoxModel
{
public:
    AudioFileListBoxModel(const std::vector<SoundFileData>& fileData)
        : mFileData(fileData)
    {
    }
    
    int getNumRows() override
    {
        return (int)mFileData.size();
    }
    
    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override
    {
        if (rowNumber >= mFileData.size())
            return existingComponentToUpdate;
        
        const auto& data = mFileData[rowNumber];
        
        auto* c = static_cast<AudioFileComponent*>(existingComponentToUpdate);
        
        if (c == nullptr)
        {
            c = new AudioFileComponent(data);
        }
        else
        {
            c->setData(&data);
        }
        
        return c;
    }
    
    void paintListBoxItem(int rowNumber,
                          Graphics& g,
                          int width, int height,
                          bool rowIsSelected) override
    {
    }
    
    const std::vector<SoundFileData>& mFileData;
};


class FilesListComponent : public Component
{
public:
    FilesListComponent(File& filesLocation, const std::vector<SoundFileData>& fileData)
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
    AudioFileListBoxModel               mListBoxModel;
    std::unique_ptr<ListBox>            mListBox;
    std::unique_ptr<FilenameComponent>  mFolderChooser;
    const int                           mRowHeight = 50;

};

//==============================================================================
/*
*/
class AudioFileListComponent    : public Component,
                                  public FilenameComponentListener
{
public:
    AudioFileListComponent(AppModel& model)
        : m(model),
          mSoundBeds(model.mCurrentSoundBedFolder, model.mSoundBedData),
          mSoundClips(model.mCurrentSoundClipFolder, model.mSoundClipData)
    {
        mSoundBeds.addListener(this);
        mSoundClips.addListener(this);
        
        addAndMakeVisible(mSoundBeds);
        addAndMakeVisible(mSoundClips);
    }

    ~AudioFileListComponent()
    {
    }

    void paint (Graphics& g) override
    {
    }

    void resized() override
    {
        auto b = getLocalBounds();
        
        mSoundBeds.setBounds(b.removeFromTop(mSoundBeds.getIdealHeight()));
        b.removeFromTop(20);
        mSoundClips.setBounds(b);
    }
    
    std::function<void()> onAudioFoldersChanged;
        
private:

    void filenameComponentChanged(FilenameComponent* component) override
    {
        if (component == mSoundBeds.getFilenameComponent())
        {
            m.mCurrentSoundBedFolder = component->getCurrentFile();
        }
        
        if (component == mSoundClips.getFilenameComponent())
        {
            m.mCurrentSoundClipFolder = component->getCurrentFile();
        }
        
        jassert(onAudioFoldersChanged != nullptr);
        onAudioFoldersChanged();
        
        mSoundClips.updateContent();
        mSoundBeds.updateContent();
    }

    AppModel&           m;
    
    FilesListComponent  mSoundBeds;
    FilesListComponent  mSoundClips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileListComponent)
};

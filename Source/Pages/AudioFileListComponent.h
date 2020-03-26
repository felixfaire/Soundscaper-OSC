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
    FilesListComponent(const String& title, File& filesLocation, const std::vector<SoundFileData>& fileData)
        : mTitle(title),
          mFilesLocation(filesLocation),
          mListBoxModel(fileData)
    {
        
        mTitleLabel.reset(new Label("FilesList", mTitle));
        mTitleLabel->setFont(mTitleLabel->getFont().withHeight(20.0f));

        mFolderChooser.reset(new FilenameComponent("Samples Folder",
                                                    mFilesLocation,
                                                    true, true, false, "", "",
                                                    "Select Samples Folder"));
        
        mListBox.reset(new ListBox("BedsList", &mListBoxModel));
        mListBox->setRowHeight((int)mRowHeight);
        mListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
    
        addAndMakeVisible(*mTitleLabel);
        addAndMakeVisible(*mFolderChooser);
        addAndMakeVisible(*mListBox);
    }
    
    void resized() override
    {
        auto b = getLocalBounds();
        mTitleLabel->setBounds(b.removeFromTop(50));
        mFolderChooser->setBounds(b.removeFromTop(30));
        mListBox->setBounds(b.reduced(0, 5));
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
    String                    mTitle;
    
    // UI
    AudioFileListBoxModel               mListBoxModel;
    std::unique_ptr<Label>              mTitleLabel;
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
          mSoundAtmospheres("Atmospheres", model.mCurrentSoundBedFolder, model.mSoundAtmosphereData),
          mSoundClips("Clips", model.mCurrentSoundClipFolder, model.mSoundClipData)
    {
        mSoundAtmospheres.addListener(this);
        mSoundClips.addListener(this);
        
        addAndMakeVisible(mSoundAtmospheres);
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
        
        mSoundAtmospheres.setBounds(b.removeFromTop(mSoundAtmospheres.getIdealHeight()));
        b.removeFromTop(20);
        mSoundClips.setBounds(b);
    }
    
    std::function<void()> onAudioFoldersChanged;
        
private:

    void filenameComponentChanged(FilenameComponent* component) override
    {
        if (component == mSoundAtmospheres.getFilenameComponent())
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
        mSoundAtmospheres.updateContent();
    }

    AppModel&           m;
    
    FilesListComponent  mSoundAtmospheres;
    FilesListComponent  mSoundClips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileListComponent)
};

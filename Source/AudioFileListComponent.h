/*
  ==============================================================================

    AudioFileListComponent.h
    Created: 12 Feb 2020 8:45:42pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FileListBoxModel : public ListBoxModel
{
public:
    FileListBoxModel(const Array<File>& files)
        : mFiles(files)
    {
    }
    
    int getNumRows() override
    {
        return mFiles.size();
    }
    
    void paintListBoxItem(int rowNumber,
                          Graphics& g,
                          int width, int height,
                          bool rowIsSelected) override
    {
        auto b = Rectangle<int>(0, 0, width, height);
        b.reduce(5, 5);
        
        g.setColour(Colour::greyLevel(0.2f));
        g.fillRoundedRectangle(b.toFloat(), 5.0f);
        
        const int margin = 10;
        b.removeFromLeft(margin);
        
        g.setColour(Colour::greyLevel(0.8f));
        
        const String name = mFiles[rowNumber].getFileNameWithoutExtension();
        g.drawText(name, b.removeFromLeft(width),
                   Justification(Justification::Flags::centredLeft), true);
    }
    
    const Array<File>& mFiles;
};


class FilesListComponent : public Component
{
public:
    FilesListComponent(File& filesLocation, const Array<File>& files)
        : mFilesLocation(filesLocation),
          mFiles(files),
          mListBoxModel(files)
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
        return (1 + mListBoxModel.getNumRows()) * mRowHeight;
    }
    
    FilenameComponent* getFilenameComponent() { return mFolderChooser.get(); }
    
private:

    // Data
    File&                     mFilesLocation;
    const Array<File>&        mFiles;
    
    // UI
    FileListBoxModel                    mListBoxModel;
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
          mSoundBeds(model.mCurrentSoundBedFolder, model.mSoundBedFiles),
          mSoundClips(model.mCurrentSoundClipFolder, model.mSoundClipFiles)
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

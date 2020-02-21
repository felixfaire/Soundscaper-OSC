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
    FileListBoxModel(AppModel& m)
        : mModel(m)
    {
    }
    
    int getNumRows() override
    {
        return mModel.mSoundFiles.size();
        
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
        
        const String name = mModel.mSoundFiles[rowNumber].getFileNameWithoutExtension();
        g.drawText(name, b.removeFromLeft(width),
                   Justification(Justification::Flags::centredLeft), true);
    }
    
    AppModel& mModel;
};

//==============================================================================
/*
*/
class AudioFileListComponent    : public Component
{
public:
    AudioFileListComponent(AppModel& model)
        : m(model),
          mListBoxModel(model)
    {
        mSamplesFolder.reset(new FilenameComponent("Samples Folder",
                                           m.mCurrentAudioFolder,
                                           true, true, false, "", "",
                                           "Select Samples Folder"));
        
        mFilesListBox.reset(new ListBox("FilesList", &mListBoxModel));
        mFilesListBox->setRowHeight(50);
        mFilesListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
        
        addAndMakeVisible(*mSamplesFolder);
        addAndMakeVisible(*mFilesListBox);
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
        mSamplesFolder->setBounds(b.removeFromTop(50).reduced(10));
        mFilesListBox->setBounds(b.reduced(5));
        
        mFilesListBox->updateContent();
    }
    
    File               getCurrentFile()   { return mSamplesFolder->getCurrentFile(); }
    FilenameComponent& getFileComponent() { return *mSamplesFolder; }

private:

    AppModel&                                     m;
    std::unique_ptr<FilenameComponent>            mSamplesFolder;
    
    FileListBoxModel                              mListBoxModel;
    std::unique_ptr<ListBox>                      mFilesListBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileListComponent)
};

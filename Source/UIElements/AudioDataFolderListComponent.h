/*
  ==============================================================================

    FileListComponent.h
    Created: 1 Apr 2020 2:28:48pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../State/AppModel.h"
#include "MinimalLookAndFeel.h"
#include "HelpButton.h"

class AudioDataFolderListComponent : public Component
{
public:
    AudioDataFolderListComponent(const String& title, File& filesLocation)
        : mTitle(title),
          mFilesLocation(filesLocation)
    {

        mTitleLabel.reset(new Label("FilesList", mTitle));
        mTitleLabel->setFont(mTitleLabel->getFont().withHeight(25.0f));

        mFolderChooser.reset(new FilenameComponent("Samples Folder",
                                                    mFilesLocation,
                                                    true, true, false, "", "",
                                                    "Select" + title + "Folder"));

        mHelpButton.reset(new HelpButton());
        
        addAndMakeVisible(*mTitleLabel);
        addAndMakeVisible(*mFolderChooser);
        addAndMakeVisible(*mHelpButton);
    }

    void paint(Graphics& g) override
    {
        auto b = getLocalBounds().reduced(1).toFloat();

        MinimalLookAndFeel::drawPanelBackground(g, b);
    }

    void resized() override
    {
        jassert(mListBox != nullptr); // Must initialise listbox first!

        auto b = getLocalBounds().reduced(5);
        auto topBar = b.removeFromTop(mRowHeight);
        mHelpButton->setBounds(topBar.removeFromRight(topBar.getHeight()).withSizeKeepingCentre(20, 20));
        mTitleLabel->setBounds(topBar);
        mFolderChooser->setBounds(b.removeFromTop(27).reduced(1));
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

    FilenameComponent* getFilenameComponent() { return mFolderChooser.get(); }

protected:

    // Data
    String                    mTitle;
    File&                     mFilesLocation;

    // UI
    std::unique_ptr<HelpButton>         mHelpButton;
    std::unique_ptr<Label>              mTitleLabel;
    std::unique_ptr<FilenameComponent>  mFolderChooser;
    std::unique_ptr<ListBox>            mListBox;   // maintained by parent class
    const int                           mRowHeight = 50;

};

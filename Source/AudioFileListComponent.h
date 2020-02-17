/*
  ==============================================================================

    AudioFileListComponent.h
    Created: 12 Feb 2020 8:45:42pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AudioFileListComponent    : public Component
{
public:
    AudioFileListComponent(AppModel& model)
        : m(model)
    {
        mSamplesFolder.reset(new FilenameComponent("Samples Folder",
                                           m.mCurrentAudioFolder,
                                           true, true, false, "", "",
                                           "Select Samples Folder"));
        
        addAndMakeVisible(*mSamplesFolder);
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
    }
    
    File               getCurrentFile()   { return mSamplesFolder->getCurrentFile(); }
    FilenameComponent& getFileComponent() { return *mSamplesFolder; }

private:

    AppModel&                                     m;
    std::unique_ptr<FilenameComponent>            mSamplesFolder;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileListComponent)
};

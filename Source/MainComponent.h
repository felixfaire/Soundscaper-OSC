/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnvironmentModel.h"
#include "IOSettingsComponent.h"
#include "AudioFileListComponent.h"
#include "UIElements/MinimalLookAndFeel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
                        public FilenameComponentListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void loadAudioFiles();
    void filenameComponentChanged(FilenameComponent* component) override;

private:
    //==============================================================================
    
    std::unique_ptr<TabbedComponent>              mTabbedContainer;
    
    std::unique_ptr<IOSettingsComponent>          mIOSettings;
    std::unique_ptr<AudioFileListComponent>       mFileList;
    
    EnvironmentModel                              mModel;
    
    Synthesiser  mSynth;
    
    AudioFormatManager  mFormatManager;
    MinimalLookAndFeel  mLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

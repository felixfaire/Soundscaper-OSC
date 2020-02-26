/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AppModel.h"
#include "AppController.h"
#include "IOSettingsComponent.h"
#include "AudioFileListComponent.h"
#include "UIElements/MinimalLookAndFeel.h"
#include "SpaceViewerComponent.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public FilenameComponentListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    bool keyPressed(const KeyPress& event) override;

    void filenameComponentChanged(FilenameComponent* component) override;

private:
    //==============================================================================
    
    std::unique_ptr<TabbedComponent>              mTabbedContainer;
    
    std::unique_ptr<IOSettingsComponent>          mIOSettings;
    std::unique_ptr<AudioFileListComponent>       mFileList;
    std::unique_ptr<SpaceViewerComponent>         mSpaceViewer;
    
    AppModel                                      mModel;
    AppController                                 mController;
    
    MinimalLookAndFeel                            mLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

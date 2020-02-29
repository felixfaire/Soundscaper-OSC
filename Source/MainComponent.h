/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppModel.h"
#include "Audio/AudioController.h"

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
                        public FilenameComponentListener,
                        public ChangeListener,
                        private OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    bool keyPressed(const KeyPress& event) override;

    // Controller
    void triggerSource(int soundID, const glm::vec3& pos);
    void updateSource(int soundID, const glm::vec3& pos);
    void allNotesOff();
    
    // Callbacks
    void filenameComponentChanged(FilenameComponent* component) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void oscMessageReceived(const OSCMessage& message) override;
        
private:
    //==============================================================================

    // Model
    AppModel                                      mModel;
    
    // Audio
    AudioController                               mAudio;

    // UI
    std::unique_ptr<TabbedComponent>              mTabbedContainer;
    
    std::unique_ptr<IOSettingsComponent>          mIOSettings;
    std::unique_ptr<AudioFileListComponent>       mFileList;
    std::unique_ptr<SpaceViewerComponent>         mSpaceViewer;

    MinimalLookAndFeel                            mLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

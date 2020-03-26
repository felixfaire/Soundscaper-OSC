/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "State/AppModel.h"
#include "Audio/AudioController.h"

#include "Pages/IOSettingsComponent.h"
#include "Pages/AudioFileListComponent.h"
#include "Pages/SpaceConfigComponent.h"

#include "UIElements/MinimalLookAndFeel.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public ChangeListener,
                        private OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>,
                        private Timer
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
    void triggerSource(int noteID, int soundID, const glm::vec3& pos);
    void updateSource(int noteID, const glm::vec3& pos);
    void allNotesOff();
    
    // Callbacks
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void oscMessageReceived(const OSCMessage& message) override;
        
private:
    //==============================================================================

    void timerCallback() override;

    // Model
    AppModel                                      mModel;
    
    // Audio
    AudioController                               mAudio;

    // UI
    std::unique_ptr<TabbedComponent>              mTabbedContainer;
    
    std::unique_ptr<IOSettingsComponent>          mIOSettings;
    std::unique_ptr<AudioFileListComponent>       mFilesListComponent;
    std::unique_ptr<SpaceConfigComponent>         mSpaceComponent;

    MinimalLookAndFeel                            mLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

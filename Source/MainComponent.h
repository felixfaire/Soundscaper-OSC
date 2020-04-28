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
#include "UIElements/ExpandingPageContainer.h"
#include "UIElements/ChannelMonitorComponent.h"

#include "UIElements/MinimalLookAndFeel.h"

//==============================================================================
/*
    Main App window component
    This also acts as the primary controller for the UI and OSC callbacks
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
    // Component
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
    std::unique_ptr<ExpandingPageContainer>       mPagesContainer;
    
    // Pages
    std::unique_ptr<IOSettingsComponent>          mIOSettings;
    std::unique_ptr<AudioFileListComponent>       mFilesListComponent;
    std::unique_ptr<SpaceConfigComponent>         mSpaceComponent;

    // Bottom Bar
    std::unique_ptr<ChannelMonitorComponentBar>   mChannelMonitorBar;

    SharedResourcePointer<TooltipWindow>          mTooltipWindow;

    MinimalLookAndFeel                            mLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

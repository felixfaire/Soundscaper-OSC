/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Init Audio
    mAudio.setAudioChannels(0, 2);
    mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositions);

    mModel.addChangeListener(this);
    mModel.mOSCReciever.addListener(this);
    
    
    // Init UI
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(mModel, mAudio.getDeviceManager()));
    mFileList.reset(new AudioFileListComponent(mModel));
    mSpaceViewer.reset(new SpaceViewerComponent(mModel));
    
    mSpaceViewer->onTrigger = [this](int index, glm::vec3 p){
        triggerSource(index, p);
    };
    
    mSpaceViewer->onUpdate = [this](int index, glm::vec3 p){
        updateSource(index, p);
    };
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("Space", Colour(), mSpaceViewer.get(), false);
    mTabbedContainer->addTab("Sounds", Colour(), mFileList.get(), false);
    mTabbedContainer->addTab("Settings", Colour(), mIOSettings.get(), false);
    mTabbedContainer->setIndent(10);
    mTabbedContainer->setTabBarDepth(50);
    mTabbedContainer->setOutline(0);
    mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());
    
    addAndMakeVisible(*mTabbedContainer);
    
    setWantsKeyboardFocus(true);
    
    setSize (800, 800);
        
    mFileList->getFileComponent().addListener(this);
    mAudio.loadAudioFiles(mModel);
    mSpaceViewer->updateFileList();
    mFileList->resized();
}

MainComponent::~MainComponent()
{
    mModel.addChangeListener(this);
}

// ===== CONTROLLER ====================================================

void MainComponent::triggerSource(int soundID, const glm::vec3& pos)
{
    jassert(soundID < mModel.mSoundFiles.size());
    const int noteID = ++mModel.mCurrentNoteID;
    mAudio.addSoundEvent({noteID, soundID, pos});
}

void MainComponent::updateSource(int soundID, const glm::vec3& pos)
{
    jassert(soundID < mModel.mSoundFiles.size());
    const int noteID = mModel.mCurrentNoteID;
    mAudio.addSoundEvent({noteID, -1, pos});
}

void MainComponent::allNotesOff()
{
    mAudio.mSynth.allNotesOff(true);
}


//===== COMPONENT =============================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colour(10, 20, 30));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    mTabbedContainer->setBounds(bounds);
}

bool MainComponent::keyPressed(const KeyPress& key)
{
    if (key == KeyPress::escapeKey)
        allNotesOff();
        
    return true;
}


// ===== CALLBACKS =====================================================
void MainComponent::filenameComponentChanged(FilenameComponent* component)
{
    mModel.mCurrentAudioFolder = component->getCurrentFile();
    mAudio.loadAudioFiles(mModel);
    mSpaceViewer->updateFileList();
    mFileList->resized();
}

void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    jassert(dynamic_cast<AppModel*>(source) != nullptr);
    
    mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositions);
}

void MainComponent::oscMessageReceived(const OSCMessage& message)
{
    // TODO: convert this to not use strings
    if (message.getAddressPattern().toString() == "/sound/position")
    {
        if (message[0].isInt32())
        {
            const int id = message[0].getInt32();
            triggerSource(id, glm::vec3(0.0f));
        }
        else
        {
            DBG("Incorrect message type");
        }
    }
}

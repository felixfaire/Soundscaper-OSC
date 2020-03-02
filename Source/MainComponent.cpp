/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"
#include "Utils/AppModelLoader.h"

//==============================================================================
MainComponent::MainComponent()
    : mAudio(mModel.mDeviceManager)
{
    // Init model
    mModel.mSpeakerPositionsChanges.addChangeListener(this);
    mModel.mOSCReciever.addListener(this);
    AppModelLoader::loadSettings(mModel);

    // Init Audio
    mAudio.initialise();
    mAudio.mSynth.updateSpeakerPositions(mModel.getSpeakerPositions());
    
    mAudio.loadAudioFiles(mModel);
    
    // Init UI
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(mModel, mAudio.getDeviceManager()));
    mFilesListComponent.reset(new AudioFileListComponent(mModel));
    mSpaceViewer.reset(new SpaceViewerComponent(mModel));
    
    mFilesListComponent->onAudioFoldersChanged = [this]()
    {
        mAudio.loadAudioFiles(mModel);
        mSpaceViewer->updateFileList(); // TODO: remove if demo not needed
    };
    
    mSpaceViewer->onTrigger = [this](int index, glm::vec3 p){
        triggerSource(index, p);
    };
    
    mSpaceViewer->onUpdate = [this](int index, glm::vec3 p){
        updateSource(index, p);
    };
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("Space", Colour(), mSpaceViewer.get(), false);
    mTabbedContainer->addTab("Sounds", Colour(), mFilesListComponent.get(), false);
    mTabbedContainer->addTab("Settings", Colour(), mIOSettings.get(), false);
    mTabbedContainer->setIndent(10);
    mTabbedContainer->setTabBarDepth(50);
    mTabbedContainer->setOutline(0);
    mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());
    
    addAndMakeVisible(*mTabbedContainer);
    
    setWantsKeyboardFocus(true);
            
    mSpaceViewer->updateFileList();
    mFilesListComponent->resized();
    
    setSize (800, 800);
}

MainComponent::~MainComponent()
{
    AppModelLoader::saveSettings(mModel);
}

// ===== CONTROLLER ====================================================

void MainComponent::triggerSource(int soundID, const glm::vec3& pos)
{
    jassert(soundID < mModel.mSoundClipData.size());
    const int noteID = ++mModel.mCurrentNoteID;
    mAudio.addSoundEvent({noteID, soundID, pos});
}

void MainComponent::updateSource(int soundID, const glm::vec3& pos)
{
    jassert(soundID < mModel.mSoundClipData.size());
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


void MainComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &mModel.mSpeakerPositionsChanges)
    {
        mSpaceViewer->updateComponentPositions();
        mAudio.mSynth.updateSpeakerPositions(mModel.getSpeakerPositions());
    }
    else if (source == &mModel.mSoundBedAmplitudesChanges)
    {
        mAudio.setSoundBedAmplitudes(mModel.getSoundBedAmpitudes());
    }
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

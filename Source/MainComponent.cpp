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
    mModel.mSoundBedAmplitudesChanges.addChangeListener(this);
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
    mSpaceComponent.reset(new SpaceConfigComponent(mModel));
    
    mFilesListComponent->onAudioFoldersChanged = [this]()
    {
        mAudio.loadAudioFiles(mModel);
    };
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("Space", Colour(), mSpaceComponent.get(), false);
    mTabbedContainer->addTab("Sounds", Colour(), mFilesListComponent.get(), false);
    mTabbedContainer->addTab("Settings", Colour(), mIOSettings.get(), false);
    mTabbedContainer->setIndent(10);
    mTabbedContainer->setTabBarDepth(50);
    mTabbedContainer->setOutline(0);
    mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());
    
    addAndMakeVisible(*mTabbedContainer);
    
    setWantsKeyboardFocus(true);

    mFilesListComponent->resized();
    
    mModel.setSoundbedAmplitude(0, 0.0f);
    
    startTimer(15);
    
    setSize (500, 800);
}

MainComponent::~MainComponent()
{
    AppModelLoader::saveSettings(mModel);
}

// ===== CONTROLLER ====================================================

void MainComponent::triggerSource(int noteID, int soundID, const glm::vec3& pos)
{
    jassert(soundID < mModel.mSoundClipData.size());

    if (soundID >= mModel.mSoundClipData.size())
        return;

    mAudio.addSoundEvent({noteID, soundID, pos});
}

void MainComponent::updateSource(int noteID, const glm::vec3& pos)
{
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


void MainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &mModel.mSpeakerPositionsChanges)
    {
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
    if (message.getAddressPattern().toString() == "/start")
    {
        if (message[0].isInt32()
         && message[1].isInt32()
         && message[2].isFloat32()
         && message[3].isFloat32()
         && message[4].isFloat32())
        {
            const int noteID = message[0].getInt32();
            const int soundID = message[1].getInt32();
            const float x = message[2].getFloat32();
            const float y = message[3].getFloat32();
            const float z = message[4].getFloat32();
            triggerSource(noteID, soundID, glm::vec3(x, y, z));
        }
        else
        {
            DBG("Incorrect message type");
        }
    }
    else if (message.getAddressPattern().toString() == "/update")
    {
        if (message[0].isInt32())
        {
            const int noteID = message[0].getInt32();
            const float x = message[1].getFloat32();
            const float y = message[2].getFloat32();
            const float z = message[3].getFloat32();
            updateSource(noteID, glm::vec3(x, y, z));
        }
        else
        {
            DBG("Incorrect message type");
        }
    }
}

// Update model audio levels on a timer
void MainComponent::timerCallback()
{
    std::vector<float> levels = mAudio.getAudioLevels();
    mModel.setAudioLevels(levels);
}

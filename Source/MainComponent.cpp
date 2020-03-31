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
    mModel.mSpeakerPositionsState.addChangeListener(this);
    mModel.mAudioDataState.addChangeListener(this);
    mModel.mAtmosphereLevelState.addChangeListener(this);
    mModel.mOSCReciever.addListener(this);
    AppModelLoader::loadSettings(mModel);

    // Init Audio
    mAudio.initialise();
    mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositionsState.getPositions());
    mAudio.loadAudioFiles(mModel.mAudioDataState);
    
    // Init UI
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(mModel, mAudio.getDeviceManager()));
    mFilesListComponent.reset(new AudioFileListComponent(mModel.mAudioDataState));
    mSpaceComponent.reset(new SpaceConfigComponent(mModel));
    
    mFilesListComponent->onAudioFoldersChanged = [this]()
    {
        mAudio.loadAudioFiles(mModel.mAudioDataState);
    };
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("Space", Colour(), mSpaceComponent.get(), false);
    mTabbedContainer->addTab("Sounds", Colour(), mFilesListComponent.get(), false);
    mTabbedContainer->addTab("Settings", Colour(), mIOSettings.get(), false);
    mTabbedContainer->setIndent(5);
    mTabbedContainer->setTabBarDepth(50);
    mTabbedContainer->setOutline(0);
    mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());

    mChannelMonitorBar.reset(new ChannelMonitorComponentBar(mModel));
    
    addAndMakeVisible(*mTabbedContainer);
    addAndMakeVisible(*mChannelMonitorBar);
    
    setWantsKeyboardFocus(true);

    mFilesListComponent->resized();
    
    startTimer(16); // animation timer
    
    setSize(500, 800);
}

MainComponent::~MainComponent()
{
    AppModelLoader::saveSettings(mModel);
}

// ===== CONTROLLER ====================================================

void MainComponent::triggerSource(int noteID, int soundID, const glm::vec3& pos)
{
    const int numClips = (int)mModel.mAudioDataState.mSoundClipData.size();

    jassert(soundID < numClips);

    if (soundID >= numClips)
        return; // TODO: push error message to app console

    // Audio
    mAudio.addSoundEvent({noteID, soundID, pos});

    // Visualisation
    const auto& fileData = mModel.mAudioDataState.mSoundClipData[soundID];
    mModel.mVisualVoiceState.addSound(noteID, fileData, pos);
}

void MainComponent::updateSource(int noteID, const glm::vec3& pos)
{
    mAudio.addSoundEvent({noteID, -1, pos});

    // Visualisation
    mModel.mVisualVoiceState.updateSound(noteID, pos);
}

void MainComponent::allNotesOff()
{
    mAudio.mSynth.allNotesOff(true);
    mModel.mVisualVoiceState.clear();
}


//===== COMPONENT =============================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colour(10, 20, 30));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto monitorBounds = bounds.removeFromBottom(25);
    mTabbedContainer->setBounds(bounds);

    monitorBounds = monitorBounds.withTrimmedBottom(5);
    mChannelMonitorBar->setBounds(monitorBounds.reduced(5, 0));
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
    if (source == &mModel.mSpeakerPositionsState)
    {
        mAudio.mSynth.updateSpeakerPositions(mModel.mSpeakerPositionsState.getPositions());
    }
    else if (source == &mModel.mAtmosphereLevelState)
    {
        mAudio.setSoundAtmosphereAmplitudes(mModel.mAtmosphereLevelState.getSoundAtmosphereAmpitudes());
    }
    else if (source == &mModel.mAudioDataState)
    {
        const int numAtmospheres = (int)mModel.mAudioDataState.mSoundAtmosphereData.size();
        const int numAtmosphereLevels = (int)mModel.mAtmosphereLevelState.getSoundAtmosphereAmpitudes().size();

        if (numAtmospheres != numAtmosphereLevels)
            mModel.mAtmosphereLevelState.resetAmplitudes(numAtmospheres);
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

// Animation timer callback
void MainComponent::timerCallback()
{
    // Update output audio levels
    std::vector<float> levels = mAudio.getAudioLevels();
    mModel.mAudioMonitorState.setAudioLevels(levels);

    // Update visual sources
    mModel.mVisualVoiceState.update();
}

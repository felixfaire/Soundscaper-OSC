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
    
    mSpaceComponent.reset(new SpaceConfigComponent(mModel));
    mFilesListComponent.reset(new AudioFileListComponent(mModel));
    mIOSettings.reset(new IOSettingsComponent(mModel, mAudio.getDeviceManager()));
    
    mPagesContainer.reset(new ExpandingPageContainer());
    mPagesContainer->addPage("Space", mSpaceComponent.get());
    mPagesContainer->addPage("Sounds", mFilesListComponent.get());
    mPagesContainer->addPage("Settings", mIOSettings.get());
    mPagesContainer->setMultiViewEnabled(false);
    addAndMakeVisible(mPagesContainer.get());
    
    mFilesListComponent->onAudioFoldersChanged = [this]()
    {
        mAudio.loadAudioFiles(mModel.mAudioDataState);
    };
    
    mChannelMonitorBar.reset(new ChannelMonitorComponentBar(mModel));
    addAndMakeVisible(*mChannelMonitorBar);
    
    setWantsKeyboardFocus(true);
    
    startTimer(16); // animation timer

    mTooltipWindow->setOpaque(false);
    mTooltipWindow->setMillisecondsBeforeTipAppears(900);
    
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

    if (soundID >= numClips || soundID < 0)
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
    // Layout components
    auto bounds = getLocalBounds();
    auto monitorBounds = bounds.removeFromBottom(22);
    mPagesContainer->setBounds(bounds);

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
        if (message[0].isInt32() // noteID
         && message[1].isInt32() // soundID
         && message[2].isFloat32() && message[3].isFloat32() && message[4].isFloat32())
        {
            const int soundID = message[0].getInt32();
            const int noteID = message[1].getInt32();
            const float x = message[2].getFloat32();
            const float y = message[3].getFloat32();
            const float z = message[4].getFloat32();
            triggerSource(noteID, soundID, glm::vec3(x, y, z));
        }
        else if (message[0].isString() // soundAddress
              && message[1].isInt32()  // noteID
              && message[2].isFloat32() && message[3].isFloat32() && message[4].isFloat32())
        {
            const int soundID = mModel.mAudioDataState.getSoundIndexFromClipAddress(message[0].getString());
            const int noteID = message[1].getInt32();
            const float x = message[2].getFloat32();
            const float y = message[3].getFloat32();
            const float z = message[4].getFloat32();
            triggerSource(noteID, soundID, glm::vec3(x, y, z));
        }
        else if (message[0].isInt32() // soundID
              && message[1].isFloat32() && message[2].isFloat32() && message[3].isFloat32())
        {
            const int noteID = -1;
            const int soundID = message[0].getInt32();
            const float x = message[1].getFloat32();
            const float y = message[2].getFloat32();
            const float z = message[3].getFloat32();
            triggerSource(noteID, soundID, glm::vec3(x, y, z));
        }
        else if (message[0].isString() // soundAddress
              && message[1].isFloat32() && message[2].isFloat32() && message[3].isFloat32())
        {
            const int noteID = -1;
            const int soundID = mModel.mAudioDataState.getSoundIndexFromClipAddress(message[0].getString());
            const float x = message[1].getFloat32();
            const float y = message[2].getFloat32();
            const float z = message[3].getFloat32();
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
    else if (message.getAddressPattern().toString() == "/atmosphere")
    {
        if (message[0].isInt32()
         && message[1].isFloat32())
        {
            const int atmosphereIndex = message[0].getInt32();
            const float level = message[1].getFloat32();
         
            mModel.mAtmosphereLevelState.setSoundAtmosphereAmplitude(atmosphereIndex, level);
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
    const auto& levels = mAudio.getAudioLevels();
    mModel.mAudioMonitorState.setAudioLevels(levels);

    // Update visual sources
    mModel.mVisualVoiceState.update();
}

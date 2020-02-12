/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(deviceManager));
    mFileList.reset(new AudioFileListComponent());
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("IO Settings", Colour::greyLevel(0.1f), mIOSettings.get(), false);
    mTabbedContainer->addTab("Audio Files", Colour::greyLevel(0.1f), mFileList.get(), false);
    
    addAndMakeVisible(*mTabbedContainer);
    
    setAudioChannels (0, 2);
    setSize (800, 800);
    
    mFormatManager.registerBasicFormats();
    
    mFileList->getFileComponent().addListener(this);
    loadAudioFiles();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    String message;
    message << "Preparing to play audio..." << newLine;
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << newLine;
    message << " sampleRate = " << sampleRate;
    Logger::getCurrentLogger()->writeToLog (message);
    
    mSynth.setCurrentPlaybackSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
//        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
//        {
//            // Get a pointer to the start sample in the buffer for this audio output channel
//            auto* buffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
//
//            // Fill the required number of samples with noise between -0.125 and +0.125
//            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
//                buffer[sample] = (mRand.nextFloat() - 0.5f) * 0.1f;
//        }
    
    MidiBuffer midi;
    mSynth.renderNextBlock(*bufferToFill.buffer, midi, bufferToFill.startSample, bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    Logger::getCurrentLogger()->writeToLog ("Releasing audio resources");
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colour(10, 20, 30));
    
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    auto bounds = getBounds();
    mTabbedContainer->setBounds(getBounds());
}

void MainComponent::mouseDown(const MouseEvent& event)
{
    const int step = (int)(mModel.mSoundFiles.size() * event.getMouseDownX() / getWidth());
    const int noteNum = 60 + step;
    mSynth.noteOn(1, noteNum, 1.0f);
    
    Logger::getCurrentLogger()->writeToLog("Playing Note: " + std::to_string(noteNum));
}

void MainComponent::loadAudioFiles()
{
    mSynth.clearSounds();
    mSynth.clearVoices();
    
    File folder = mFileList->getCurrentFile();
    mModel.mSoundFiles = folder.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.wav");
    
    int noteNum = 60;
    
    for (auto& wavFile : mModel.mSoundFiles)
    {
        std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(wavFile));
        
        if (reader != nullptr)
        {
            Logger::getCurrentLogger()->writeToLog("Loading file: " + wavFile.getFileNameWithoutExtension());
            auto notes = BigInteger();
            notes.setBit(noteNum, true);
            auto* newSound = new SamplerSound(wavFile.getFileNameWithoutExtension(), *reader, notes, noteNum, 0.01, 10.0, 20.0);
            mSynth.addSound(newSound);
            mSynth.addVoice(new SamplerVoice());
            noteNum++;
        }
    }
    
    if (mModel.mSoundFiles.size() == 0)
    {
        Logger::getCurrentLogger()->writeToLog("Failed to find any .wavs");
    }
}

void MainComponent::filenameComponentChanged(FilenameComponent* component)
{
    loadAudioFiles();
}

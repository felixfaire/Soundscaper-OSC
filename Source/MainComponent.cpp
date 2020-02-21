/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : mController(mModel)
{
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(mController.getDeviceManager()));
    mFileList.reset(new AudioFileListComponent(mModel));
    mDemoPlayer.reset(new DemoVoicePlayerComponent(mModel));
    
    mDemoPlayer->onTrigger = [this](int index, glm::vec3 p){
        mController.triggerSource(index, p);
    };
    
    mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
    mTabbedContainer->addTab("IO Settings", Colour(), mIOSettings.get(), false);
    mTabbedContainer->addTab("Audio Files", Colour(), mFileList.get(), false);
    mTabbedContainer->addTab("Demo Player", Colour(), mDemoPlayer.get(), false);
    mTabbedContainer->setIndent(10);
    mTabbedContainer->setTabBarDepth(50);
    mTabbedContainer->setOutline(0);
    mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());
    
    addAndMakeVisible(*mTabbedContainer);
    
    setSize (800, 800);
        
    mFileList->getFileComponent().addListener(this);
    mController.loadAudioFiles();
    mDemoPlayer->updateFileList();
    mFileList->resized();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (Colour(10, 20, 30));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    mTabbedContainer->setBounds(bounds);
}

void MainComponent::mouseDown(const MouseEvent& event)
{
    
}

void MainComponent::filenameComponentChanged(FilenameComponent* component)
{
    mModel.mCurrentAudioFolder = component->getCurrentFile();
    mController.loadAudioFiles();
    mDemoPlayer->updateFileList();
    mFileList->resized();
}

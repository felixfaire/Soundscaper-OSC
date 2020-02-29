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
    // Init Audio
    // TODO: put controller stuff in here so the controller can take over the ui?
    
    MinimalLookAndFeel::setDefaultLookAndFeel(&mLookAndFeel);
    
    mIOSettings.reset(new IOSettingsComponent(mModel, mController.getDeviceManager()));
    mFileList.reset(new AudioFileListComponent(mModel));
    mSpaceViewer.reset(new SpaceViewerComponent(mModel));
    
    mSpaceViewer->onTrigger = [this](int index, glm::vec3 p){
        mController.triggerSource(index, p);
    };
    
    mSpaceViewer->onUpdate = [this](int index, glm::vec3 p){
        mController.updateSource(index, p);
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
    mController.loadAudioFiles();
    mSpaceViewer->updateFileList();
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

bool MainComponent::keyPressed(const KeyPress& key)
{
    if (key == KeyPress::escapeKey)
        mController.allNotesOff();
        
    return true;
}

void MainComponent::filenameComponentChanged(FilenameComponent* component)
{
    mModel.mCurrentAudioFolder = component->getCurrentFile();
    mController.loadAudioFiles();
    mSpaceViewer->updateFileList();
    mFileList->resized();
}

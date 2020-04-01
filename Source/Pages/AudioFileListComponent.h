/*
  ==============================================================================

    AudioFileListComponent.h
    Created: 12 Feb 2020 8:45:42pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../UIElements/AtmosphereFolderListComponent.h"
#include "../UIElements/ClipFolderListComponent.h"


//==============================================================================
/*
*/
class AudioFileListComponent    : public Component,
                                  public FilenameComponentListener
{
public:
    AudioFileListComponent(AppModel& model)
        : mModel(model)
    {
        mSoundAtmospheres.reset(new AtmosphereFolderListComponent(mModel, "Atmospheres"));
        mSoundClips.reset(new ClipFolderListComponent(mModel, "Sound Clips"));

        mSoundAtmospheres->addListener(this);
        mSoundClips->addListener(this);
        
        addAndMakeVisible(*mSoundAtmospheres);
        addAndMakeVisible(*mSoundClips);
    }

    ~AudioFileListComponent()
    {
    }

    void paint (Graphics& g) override
    {
    }

    void resized() override
    {
        auto b = getLocalBounds();
        
        const int atmosphereHeight = jmin(b.getHeight() / 2, mSoundAtmospheres->getIdealHeight());

        mSoundAtmospheres->setBounds(b.removeFromTop(atmosphereHeight));
        b.removeFromTop(10);
        mSoundClips->setBounds(b);
    }
    
    std::function<void()> onAudioFoldersChanged;
        
private:

    void filenameComponentChanged(FilenameComponent* component) override
    {
        if (component == mSoundAtmospheres->getFilenameComponent())
        {
            mModel.mAudioDataState.mCurrentSoundAtmosphereFolder = component->getCurrentFile();
        }
        
        if (component == mSoundClips->getFilenameComponent())
        {
            mModel.mAudioDataState.mCurrentSoundClipFolder = component->getCurrentFile();
        }
        
        jassert(onAudioFoldersChanged != nullptr);
        onAudioFoldersChanged();
        
        mSoundClips->updateContent();
        mSoundAtmospheres->updateContent();
        resized();
    }

    AppModel&     mModel;
    
    std::unique_ptr<AtmosphereFolderListComponent>  mSoundAtmospheres;
    std::unique_ptr<ClipFolderListComponent>        mSoundClips;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFileListComponent)
};

/*
  ==============================================================================

    AtmosphereFolderListComponent.h
    Created: 1 Apr 2020 2:27:35pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../State/AppModel.h"
#include "MinimalLookAndFeel.h"
#include "AudioDataFolderListComponent.h"

#include "NamedWaveformComponent.h"


class AtmosphereListItemComponent : public Component,
                                    public ChangeListener
{
public:
    AtmosphereListItemComponent(AppModel& model, int index)
        : mModel(model),
          mCurrentIndex(index)
    {
        mNamedWaveformComponent.reset(new NamedWaveformComponent());
        mLevelSlider.reset(new Slider());
        mLevelSlider->setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        mLevelSlider->setTextBoxStyle(Slider::NoTextBox, true, 1, 1);
        mLevelSlider->setRange(0.0, 1.0);
        mLevelSlider->setTooltip("volume");
        mLevelSlider->onValueChange = [this]() {
            mModel.mAtmosphereLevelState.setSoundAtmosphereAmplitude(mCurrentIndex, (float)mLevelSlider->getValue());
        };

        addAndMakeVisible(*mNamedWaveformComponent);
        addAndMakeVisible(*mLevelSlider);

        setIndex(index);

        mModel.mAtmosphereLevelState.addChangeListener(this);
    }

    void paint(Graphics& g) override
    {
        auto b = getLocalBounds();
        MinimalLookAndFeel::drawFileListItemBackground(g, b);
    }

    void resized() override
    {
        auto b = getLocalBounds();

        mLevelSlider->setBounds(b.removeFromRight(b.getHeight()));
        mNamedWaveformComponent->setBounds(b.reduced(3, 7));
    }

    void setIndex(int index) 
    {
        jassert(index < mModel.mAudioDataState.mSoundAtmosphereData.size());
        jassert(index < mModel.mAtmosphereLevelState.getSoundAtmosphereAmpitudes().size());

        mCurrentIndex = index;

        // Update audio file component
        const auto& fileList = mModel.mAudioDataState.mSoundAtmosphereData;
        const auto* fileData = &fileList[index];
        mNamedWaveformComponent->setData(fileData);

        // Update Slider
        updateSliderValue();
    }

private:

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        updateSliderValue();
    }

    void updateSliderValue()
    {
        if (mCurrentIndex >= mModel.mAtmosphereLevelState.getSoundAtmosphereAmpitudes().size())
            return;

        const float level = mModel.mAtmosphereLevelState.getSoundAtmosphereAmpitude(mCurrentIndex);
        mLevelSlider->setValue(level, dontSendNotification);
    }

    AppModel&                           mModel;

    int                                     mCurrentIndex;
    std::unique_ptr<Slider>                 mLevelSlider;
    std::unique_ptr<NamedWaveformComponent> mNamedWaveformComponent;
    
};

//==============================================================================
/*
*/
class AtmosphereListBoxModel : public ListBoxModel
{
public:
    AtmosphereListBoxModel(AppModel& model)
        : mModel(model)
    {

    }

    int getNumRows() override
    {
        return (int)mModel.mAudioDataState.mSoundAtmosphereData.size();
    }

    Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override
    {
        if (rowNumber >= (int)mModel.mAudioDataState.mSoundAtmosphereData.size())
            return existingComponentToUpdate;

        auto* c = static_cast<AtmosphereListItemComponent*>(existingComponentToUpdate);

        if (c == nullptr)
        {
            c = new AtmosphereListItemComponent(mModel, rowNumber);
        }
        else
        {
            c->setIndex(rowNumber);
        }

        return c;
    }

    void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override {}

private:

    AppModel& mModel;
};


//==============================================================================
/*
*/
class AtmosphereFolderListComponent : public AudioDataFolderListComponent
{
public:
    AtmosphereFolderListComponent(AppModel& model, const String& title)
        : AudioDataFolderListComponent(title, model.mAudioDataState.mCurrentSoundAtmosphereFolder),
          mListBoxModel(model)
    {
        mListBox.reset(new ListBox("AtmospheresList", &mListBoxModel));
        mListBox->setRowHeight((int)mRowHeight);
        mListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
        addAndMakeVisible(*mListBox);
        
        mHelpButton->setTitle("Sound Atmospheres");
        mHelpButton->setText("These sounds are looped and play continuously through all speakers. They can be faded via OSC or using the onscreen dials for testing. See the instructions readme for more info on OSC message formats.\n\nNote: Multichannel files will assign channels to speakers in a repeating pattern. ");
    }

    int getIdealHeight()
    {
        return (2 + mListBoxModel.getNumRows()) * mRowHeight;
    }

private:
    
    AtmosphereListBoxModel   mListBoxModel;

};

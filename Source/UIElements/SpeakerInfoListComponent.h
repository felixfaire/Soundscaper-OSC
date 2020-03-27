/*
  ==============================================================================

    SpeakerInfoListView.h
    Created: 24 Mar 2020 4:14:02pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../State/AppModel.h"

/** A simple textbox input for a float value
*/
class FloatInputComponent : public Component,
                            public ChangeBroadcaster
{
public:
    FloatInputComponent(std::string name, float startVal, float min = -100.0f, float max = 100.0f)
        : mValue(startVal),
          mMin(min),
          mMax(max)
    {
        mNameLabel.setText(name + ":", NotificationType::dontSendNotification);
        mNameLabel.setJustificationType(Justification(Justification::centredRight));

        mValueLabel.setEditable(true);
        mValueLabel.onTextChange = [this]() {

            const auto str = mValueLabel.getText();
            
            if(str.containsOnly("0123456789.-"))
                setValue(str.getFloatValue());
            else
                setValue(0.0f);
        };

        setValue(startVal);

        addAndMakeVisible(mNameLabel);
        addAndMakeVisible(mValueLabel);
    }

    void setValue(float newValue)
    {
        if (newValue != mValue)
            sendChangeMessage();

        mValue = glm::clamp(newValue, mMin, mMax);
        mValueLabel.setText(String(mValue, 2), NotificationType::dontSendNotification);
    }

    float getValue() const { return mValue; }

    void resized()
    {
        auto b = getLocalBounds();
        const float nameWidth = mNameLabel.getFont().getStringWidth(mNameLabel.getText()) + 10.0f;

        mNameLabel.setBounds(b.removeFromLeft(nameWidth));
        mValueLabel.setBounds(b.reduced(0, b.getHeight() / 5));
    }

    Label mNameLabel;
    Label mValueLabel;
    float mValue = 0.0f;
    float mMin = -99999.0f;
    float mMax = 99999.0f;
};


/** A collection of 3 FloatInputComponents arranged horizontally for x, y, z
*/
class PositionInputComponent : public Component,
                               public ChangeListener
{
public:
    PositionInputComponent(const glm::vec3& initialPos)
    {
        mPositionLabels[0].reset(new FloatInputComponent("x", initialPos.x));
        mPositionLabels[1].reset(new FloatInputComponent("y", initialPos.y));
        mPositionLabels[2].reset(new FloatInputComponent("z", initialPos.z));
        
        for (auto& l : mPositionLabels)
        {
            l->addChangeListener(this);
            addAndMakeVisible(l.get());
        }
    }

    void resized() override
    {
        auto b = getLocalBounds();
        const auto width = b.getWidth() / 3;

        for (auto& l : mPositionLabels)
            l->setBounds(b.removeFromLeft(width));
    }

    void setPosition(const glm::vec3& pos)
    {
        for (int i = 0; i < 3; ++i)
            mPositionLabels[i]->setValue(pos[i]);
    }

    std::function<void(glm::vec3)> onPositionChanged;

private:

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        glm::vec3 pos = glm::vec3(mPositionLabels[0]->getValue(),
                                  mPositionLabels[1]->getValue(),
                                  mPositionLabels[2]->getValue());

        if (onPositionChanged != nullptr)
            onPositionChanged(pos);
    }

    std::unique_ptr<FloatInputComponent> mPositionLabels [3];
};


/** A List item for seeing and editing a speaker position
*/
class SpeakerPostionComponent : public Component
{
public:
    SpeakerPostionComponent(AppModel& m, int index)
        : mModel(m),
          mIndex(index),
          mPositionInput(m.getSpeakerPosition(index))
    {
        setIndex(index);

        mPositionInput.onPositionChanged = [this](glm::vec3 pos) {
            mModel.setSpeakerPosition(mIndex, pos);
        };

        addAndMakeVisible(mIndexLabel);
        addAndMakeVisible(mPositionInput);
    }

    void paint(Graphics& g) override
    {
        auto b = getLocalBounds();
        b.reduce(2, 5);

        g.setColour(Colour::greyLevel(0.2f));
        g.drawRoundedRectangle(b.toFloat(), 5.0f, 1.0f);
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced(7, 0);
        
        mIndexLabel.setBounds(b.removeFromLeft(100));
        mPositionInput.setBounds(b);
    }

    void setIndex(int newIndex) 
    { 
        mIndex = newIndex; 
        mIndexLabel.setText("Channel: " + String(mIndex + 1), NotificationType::dontSendNotification);
        mPositionInput.setPosition(mModel.getSpeakerPosition(mIndex));
    }

private:

    AppModel& mModel;
    int       mIndex;

    Label                  mIndexLabel;
    PositionInputComponent mPositionInput;

};


/** A ListboxModel to generate the speaker positions editor list
*/
class SpeakerInfoListBoxModel : public ListBoxModel
{
public:
    SpeakerInfoListBoxModel(AppModel& m)
        : mModel(m)
    {
    }

    int getNumRows() override
    {
        return (int)mModel.getSpeakerPositions().size();
    }

    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override
    {
        if (rowNumber >= mModel.getSpeakerPositions().size())
            return existingComponentToUpdate;

        auto* c = static_cast<SpeakerPostionComponent*>(existingComponentToUpdate);

        if (c == nullptr)
            c = new SpeakerPostionComponent(mModel, rowNumber);
        else
            c->setIndex(rowNumber);

        return c;
    }

    void paintListBoxItem(int rowNumber,
        Graphics& g,
        int width, int height,
        bool rowIsSelected) override
    {
    }

    AppModel& mModel;
};



/** A component containing the list view of the speaker positions.
*/
class SpeakerInfoListComponent : public Component,
                                 public ChangeListener
{
public:
    SpeakerInfoListComponent(AppModel& m)
        : mModel(m),
          mListBoxModel(m)
    {
        mListBox.reset(new ListBox("SpeakerList", &mListBoxModel));
        mListBox->setRowHeight((int)mRowHeight);
        mListBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);

        mModel.mSpeakerPositionsChanges.addChangeListener(this);

        addAndMakeVisible(*mListBox);
    }

    ~SpeakerInfoListComponent()
    {
        mModel.mSpeakerPositionsChanges.removeChangeListener(this);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        mListBox->setBounds(b);
        mListBox->updateContent();
    }


    void updateContent()
    {
        mListBox->updateContent();
    }

private:

    void changeListenerCallback(ChangeBroadcaster* source) override
    {
        mListBox->updateContent();
    }

    AppModel&                           mModel;

    // UI
    SpeakerInfoListBoxModel             mListBoxModel;
    std::unique_ptr<ListBox>            mListBox;
    const int                           mRowHeight = 50;

};

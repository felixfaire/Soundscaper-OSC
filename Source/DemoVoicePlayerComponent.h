/*
  ==============================================================================

    DemoVoicePlayerComponent.h
    Created: 13 Feb 2020 8:19:31pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DemoVoicePlayerComponent    : public Component
{
public:
    DemoVoicePlayerComponent(AppModel& model)
        : m(model)
    {
        addAndMakeVisible(mDemoFileBox);
    }

    ~DemoVoicePlayerComponent()
    {
    }

    void paint (Graphics& g) override
    {
        auto b = getLocalBounds().reduced(10).toFloat();
        b.removeFromTop(mBoxHeight);
        
        g.setColour(Colour::greyLevel(0.1f));
        g.fillRoundedRectangle(b, 4.0f);
        
        g.setColour(Colour::greyLevel(0.8f));
        
        for (auto& s : m.mSpeakers)
        {
            const float x = jmap(s.position.x, -1.0f, 1.0f, 0.0f, (float)getWidth());
            const float y = jmap(s.position.y, -1.0f, 1.0f, 0.0f, (float)getHeight());
            
            g.drawEllipse(x, y, 20.0f, 20.0f, 2.0f);
        }
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced(10);
        mDemoFileBox.setBounds(b.removeFromTop(mBoxHeight));
    }
    
    void mouseDown(const MouseEvent& event) override
    {
        const int index = (int)mDemoFileBox.getSelectedId() - 1;
        const glm::vec3 position = glm::vec3(jmap((float)event.getMouseDownX(), 0.0f, (float)getWidth(), -1.0f, 1.0f),
                                             0.0f,
                                             jmap((float)event.getMouseDownY(), 0.0f, (float)getHeight(), -1.0f, 1.0f));
        jassert(index < m.mSoundFiles.size());
        
        if (onTrigger != nullptr)
            onTrigger(index, position);
    }
    
    void updateFileList()
    {
        mDemoFileBox.clear();
        
        for (int i = 0; i < m.mSoundFiles.size(); ++i)
            mDemoFileBox.addItem(m.mSoundFiles[i].getFileNameWithoutExtension(), i + 1);
        
        if (m.mSoundFiles.size() > 0)
            mDemoFileBox.setSelectedId(1);
    }
    
    std::function<void(int, glm::vec3)> onTrigger;

private:

    ComboBox  mDemoFileBox;
    int       mBoxHeight = 40;
    AppModel& m;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoVoicePlayerComponent)
};

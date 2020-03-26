/*
  ==============================================================================

    ComponentContainer.h
    Created: 26 Mar 2020 5:40:40pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

/** This class simply contains and displays a content component owned elsewhere
*/
class ComponentContainer : public Component
{
public:
    ComponentContainer()
        : mContent(nullptr)
    {
    }

    void resized() override
    {
        if (mContent != nullptr)
        {
            mContent->setBounds(getLocalBounds());
        }
    }

    void setContent(Component* content) 
    { 
        jassert(content != nullptr);

        if (mContent != nullptr)
        {
            mContent->setVisible(false);
            removeChildComponent(mContent);
        }

        mContent = content;
        addAndMakeVisible(*mContent);
        mContent->setBounds(getLocalBounds());
    }

private:

    Component* mContent;
};
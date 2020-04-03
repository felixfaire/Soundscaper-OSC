/*
  ==============================================================================

    ExpandingPageContainer.h
    Created: 3 Apr 2020 11:59:03am
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/** This class contains the different pages of the app
    (which are owned by the MainComponent). It solely
    handles the layout of the pages and allows for Tabbed
    view of Multi view based on the window aspect ratio.
*/
class ExpandingPageContainer : public Component
{
public:
    ExpandingPageContainer()
    {
    }
    
    void addPage(const String& name, Component* component)
    {
        mPages.emplace_back(name, component);
    }
    
    void paint(Graphics& g) override
    {
        if (!mMultiViewEnabled)
            return;
            
        auto b = getLocalBounds().removeFromTop(mTitleHeight);
        const float step = b.getWidth() / mPages.size();
        
        g.setColour(Colour::greyLevel(0.9f).withAlpha(0.8f));
        
        for (const auto& p : mPages)
        {
            g.setFont(b.getHeight() * 0.5f);
            g.drawText(p.first, b.removeFromLeft(step), Justification(Justification::centred));
        }
    }
    
    void resized() override
    {
        const float newAspect = (float)getWidth() / (float)getHeight();

        if (newAspect > mAspectThresh && mAspect <= mAspectThresh)
            setMultiViewEnabled(true);
        else if (newAspect < mAspectThresh && mAspect >= mAspectThresh)
            setMultiViewEnabled(false);

        mAspect = newAspect;
        
        layoutPages(getLocalBounds());
    }

    void setMultiViewEnabled(bool enabled)
    {
        if (enabled)
        {
            mTabbedContainer.reset();
            
            for (const auto& page : mPages)
                addAndMakeVisible(page.second);
        }
        else
        {
            for (const auto& page : mPages)
                removeChildComponent(page.second);

            mTabbedContainer.reset(new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
            mTabbedContainer->setIndent(5);
            mTabbedContainer->setTabBarDepth(mTitleHeight);
            mTabbedContainer->setOutline(0);
            mTabbedContainer->setColour(TabbedComponent::ColourIds::backgroundColourId, Colour());
            
            for (const auto& page : mPages)
                mTabbedContainer->addTab(page.first, Colour(), page.second, false);
            
            addAndMakeVisible(*mTabbedContainer);
        }

        mMultiViewEnabled = enabled;
    }

private:
    
    void layoutPages(Rectangle<int> bounds)
    {
        if (mMultiViewEnabled)
        {
            bounds.removeFromTop(mTitleHeight);
            const int inset = 5;
            bounds.reduce(inset, inset);
            int step = bounds.getWidth() / mPages.size() - inset;
            
            for (int i = 0; i < mPages.size(); ++i)
            {
                mPages[i].second->setBounds(bounds.removeFromLeft(step));
                bounds.removeFromLeft(inset);
            }
        }
        else
        {
            mTabbedContainer->setBounds(bounds);
        }
    }

    float                                      mAspect = 1.0f;
    float                                      mAspectThresh = 1.4f;
    bool                                       mMultiViewEnabled = false;
    float                                      mTitleHeight = 50;

    std::unique_ptr<TabbedComponent>           mTabbedContainer;
    std::vector<std::pair<String, Component*>> mPages;
    
};

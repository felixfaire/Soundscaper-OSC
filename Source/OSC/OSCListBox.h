/*
  ==============================================================================

    OSCListBox.h
    Created: 24 Feb 2020 11:17:48am
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "juce_osc/juce_osc.h"

//==============================================================================
class OSCLogListBox    : public ListBox,
                         private ListBoxModel,
                         private AsyncUpdater
{
public:
    OSCLogListBox()
    {
        setModel(this);
    }

    ~OSCLogListBox() override = default;

    //==============================================================================
    int getNumRows() override
    {
        return mOscLogList.size();
    }

    //==============================================================================
    void paintListBoxItem(int row, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        ignoreUnused(rowIsSelected);

        if (isPositiveAndBelow(row, mOscLogList.size()))
        {
            const int itemIndex =(mStartEntry + row) % getNumRows();

            g.setColour(Colours::white);

            g.drawText(mOscLogList[itemIndex],
                        Rectangle<int>(width, height).reduced(4, 0),
                        Justification::centredLeft, true);
        }
    }

    //==============================================================================
    void addOSCMessage(const OSCMessage& message, int level = 0)
    {
        addEntry(getIndentationString(level)
            + "- "
            + message.getAddressPattern().toString()
            + ", "
            + String(message.size())
            + " argument(s)");

        if (!message.isEmpty())
        {
            for(auto& arg : message)
                addOSCMessageArgument(arg, level + 1);
        }

        triggerAsyncUpdate();
    }

    //==============================================================================
    void addOSCBundle(const OSCBundle& bundle, int level = 0)
    {
        OSCTimeTag timeTag = bundle.getTimeTag();

        addEntry(getIndentationString(level)
            + "- osc bundle, time tag = "
            + timeTag.toTime().toString(true, true, true, true));

        for(auto& element : bundle)
        {
            if (element.isMessage())
                addOSCMessage(element.getMessage(), level + 1);
            else if (element.isBundle())
                addOSCBundle(element.getBundle(), level + 1);
        }

        triggerAsyncUpdate();
    }

    //==============================================================================
    void addOSCMessageArgument(const OSCArgument& arg, int level)
    {
        String typeAsString;
        String valueAsString;

        if (arg.isFloat32())
        {
            typeAsString = "float32";
            valueAsString = String(arg.getFloat32());
        }
        else if (arg.isInt32())
        {
            typeAsString = "int32";
            valueAsString = String(arg.getInt32());
        }
        else if (arg.isString())
        {
            typeAsString = "string";
            valueAsString = arg.getString();
        }
        else if (arg.isBlob())
        {
            typeAsString = "blob";
            auto& blob = arg.getBlob();
            valueAsString = String::fromUTF8((const char*) blob.getData(),(int) blob.getSize());
        }
        else
        {
            typeAsString = "(unknown)";
        }

        addEntry(getIndentationString(level + 1) + "- " + typeAsString.paddedRight(' ', 12) + valueAsString);
    }

    //==============================================================================
    void addInvalidOSCPacket(const char* /* data */, int dataSize)
    {
        addEntry("-(" + String(dataSize) + "bytes with invalid format)");
    }

    //==============================================================================
    void clear()
    {
        mOscLogList.clear();
        mStartEntry = 0;
        triggerAsyncUpdate();
    }

    //==============================================================================
    void handleAsyncUpdate() override
    {
        updateContent();
        scrollToEnsureRowIsOnscreen(mOscLogList.size() - 1);
        repaint();
    }

private:

    void addEntry(const String& entry)
    {
        mOscLogList.set(mStartEntry, entry);
        mStartEntry =(mStartEntry + 1) % mMaxNumEntries;
    }

    static String getIndentationString(int level)
    {
        return String().paddedRight(' ', 2 * level);
    }

    //==============================================================================
    int             mMaxNumEntries = 100;
    int             mStartEntry = 0;
    StringArray     mOscLogList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCLogListBox)
};
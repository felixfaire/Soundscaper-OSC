/*
  ==============================================================================

    OSCSettings.h
    Created: 24 Feb 2020 11:21:50am
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "juce_osc/juce_osc.h"
#include "OSCListBox.h"


//==============================================================================
class OSCSettingsComponent   : public Component,
                               private Label::Listener,
                               private OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
    //==============================================================================
    OSCSettingsComponent(OSCReceiver& reciever)
        : mOscReceiver(reciever)
    {
        mPortNumberLabel.setJustificationType(Justification::centredRight);
        addAndMakeVisible(mPortNumberLabel);

        mPortNumberField.setEditable (true, true, true);
        mPortNumberField.addListener(this);
        mPortNumberField.setColour(Label::ColourIds::outlineColourId, Colour(0xff8e989b));
        addAndMakeVisible(mPortNumberField);

        addAndMakeVisible(mConnectButton);
        mConnectButton.onClick = [this] { connectButtonClicked(); };

        addAndMakeVisible(mClearButton);
        mClearButton.onClick = [this] { clearButtonClicked(); };

        updateConnectionStatusLabel();
        addAndMakeVisible(mConnectionStatusLabel);

        addAndMakeVisible(mOscLogListBox);
//        addAndMakeVisible(mDemoSender);

        mOscReceiver.addListener(this);
        mOscReceiver.registerFormatErrorHandler([this] (const char* data, int dataSize)
        {
            mOscLogListBox.addInvalidOSCPacket(data, dataSize);
        });

        connect();
    }

    void resized() override
    {
        auto b = getLocalBounds();
        b.removeFromTop(15);
        b.removeFromRight((int)proportionOfWidth(0.05f));

        // Demo sender
//        mDemoSender.setBounds(b.removeFromBottom(32).reduced(0, 5));

        auto topBar = b.removeFromTop(50);
        
        b.removeFromTop(5);
        const float leftLabelWidth = proportionOfWidth(0.35f); // Same as AudioDeviceSelector
        
        auto statusBox = topBar.removeFromTop(25);
        statusBox.removeFromLeft(leftLabelWidth);
        
        mConnectionStatusLabel.setBounds(statusBox);
        mPortNumberLabel.setBounds(topBar.removeFromLeft(leftLabelWidth));
        mPortNumberField.setBounds(topBar.removeFromLeft(80));
        mClearButton.setBounds(topBar.removeFromRight(60));
        topBar.removeFromRight(5);
        mConnectButton.setBounds(topBar.removeFromRight(110).reduced(5, 0));
        
        b.removeFromLeft((int)leftLabelWidth);
        mOscLogListBox.setBounds(b);

    }

private:
    //==============================================================================
    Label           mPortNumberLabel    { {}, "UDP Port Number: " };
    Label           mPortNumberField    { {}, "9001" };
    TextButton      mConnectButton      { "Connect" };
    TextButton      mClearButton        { "Clear" };
    Label           mConnectionStatusLabel;

    OSCReceiver&    mOscReceiver;
    OSCLogListBox   mOscLogListBox;

    int             mCurrentPortNumber = -1;

    //==============================================================================
    void connectButtonClicked()
    {
        if (! isConnected())
            connect();
        else
            disconnect();
    }

    //==============================================================================
    void clearButtonClicked()
    {
        mOscLogListBox.clear();
    }

    //==============================================================================
    void labelTextChanged(Label* label) override
    {
        if (label == &mPortNumberField)
        {
            // Attempt to recconect if already connected to the old port
            if (isConnected())
                connect();
        }
    }

    //==============================================================================
    void oscMessageReceived(const OSCMessage& message) override
    {
        mOscLogListBox.addOSCMessage(message);
    }

    void oscBundleReceived(const OSCBundle& bundle) override
    {
        mOscLogListBox.addOSCBundle(bundle);
    }

    //==============================================================================
    void connect()
    {
        auto portToConnect = mPortNumberField.getText().getIntValue();

        if (!isValidOscPort(portToConnect))
        {
            handleInvalidPortNumberEntered();
            return;
        }

        if (mOscReceiver.connect(portToConnect))
        {
            mCurrentPortNumber = portToConnect;
            mConnectButton.setButtonText("Disconnect");
        }
        else
        {
            handleConnectError(portToConnect);
        }

        updateConnectionStatusLabel();
    }

    //==============================================================================
    void disconnect()
    {
        if (mOscReceiver.disconnect())
        {
            mCurrentPortNumber = -1;
            mConnectButton.setButtonText("Connect");
        }
        else
        {
            handleDisconnectError();
        }

        updateConnectionStatusLabel();
    }

    //==============================================================================
    void handleConnectError(int failedPort)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "OSC Connection error",
            "Error: could not connect to port " + String(failedPort),
            "OK");
    }

    //==============================================================================
    void handleDisconnectError()
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Unknown error",
            "An unknown error occurred while trying to disconnect from UDP port.",
            "OK");
    }

    //==============================================================================
    void handleInvalidPortNumberEntered()
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
            "Invalid port number",
            "Error: you have entered an invalid UDP port number.",
            "OK");
    }

    //==============================================================================
    bool isConnected() const
    {
        return mCurrentPortNumber != -1;
    }

    //==============================================================================
    bool isValidOscPort(int port) const
    {
        return port > 0 && port < 65536;
    }

    //==============================================================================
    void updateConnectionStatusLabel()
    {
        String text = "";

        if (isConnected())
            text += "Connected to UDP port " + String(mCurrentPortNumber);
        else
            text += "Disconnected";

        auto textColour = Colour::greyLevel(0.8f);//isConnected() ? Colours::green : Colours::red;

        mConnectionStatusLabel.setText(text, dontSendNotification);
        mConnectionStatusLabel.setFont(Font(15.00f, Font::bold));
        mConnectionStatusLabel.setColour(Label::textColourId, textColour);
        mConnectionStatusLabel.setJustificationType(Justification::centredLeft);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OSCSettingsComponent)
};

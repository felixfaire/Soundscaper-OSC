/*
  ==============================================================================

    SoundEventData.h
    Created: 24 Feb 2020 10:16:27pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once


struct SoundEvent
{
    int          noteID = 0;
    int          soundID = -1;
    glm::vec3    position;
    
    bool isStartNote() const { return soundID >= 0; }
};


/** This structure manages the passing of event messages from the message
    thread onto the audio thread in a lock free way.
*/
struct SoundEventData
{
public:
    SoundEventData()
        : mFifo(FIFO_SIZE)
    {
        for (int i = 0; i < FIFO_SIZE; ++i)
        {
            mEventBuffer[i].noteID = 0;
            mEventBuffer[i].soundID = -1;
            mEventBuffer[i].position = glm::vec3(0.0f);
        }
    }
    
    static constexpr int FIFO_SIZE = 2048;

    void addSoundEvent(const SoundEvent& newEvent) // Message Thread Accessible
    {
        jassert (juce::MessageManager::getInstance()->isThisTheMessageThread());

        int start1, size1, start2, size2;
        mFifo.prepareToWrite(1, start1, size1, start2, size2);

        if (size1 > 0)
            mEventBuffer[start1] = newEvent;
        else if (size2 > 0)
            mEventBuffer[start2] = newEvent;

        mFifo.finishedWrite(size1 + size2);
    }
    
    void processEventData()                           // Audio Thread Accessible
    {
        jassert(onProcessEvent != nullptr);
        
        int start1, size1, start2, size2;
        mFifo.prepareToRead(FIFO_SIZE, start1, size1, start2, size2);

        for (int i = 0; i != size1; ++i)
            onProcessEvent(mEventBuffer[start1 + i]);

        for (int i = 0; i != size2; ++i)
            onProcessEvent(mEventBuffer[start2 + i]);

        mFifo.finishedRead(size1 + size2);
    }
    
    std::function<void(const SoundEvent& e)> onProcessEvent;

private:
    juce::AbstractFifo mFifo;
    SoundEvent         mEventBuffer[FIFO_SIZE];
    
};

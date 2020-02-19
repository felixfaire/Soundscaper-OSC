/*
  ==============================================================================

    SpatialSynthSound.h
    Created: 19 Feb 2020 10:36:53pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
    Describes one of the sounds that a SpatialSynth can play.

    A synthesiser can contain one or more sounds, and a sound can choose which
    midi notes and channels can trigger it.

    The SpatialSynthSound is a passive class that just describes what the sound is -
    the actual audio rendering for a sound is done by a SpatialSynthVoice. This allows
    more than one SpatialSynthVoice to play the same sound at the same time.

    @see SpatialSynth, SpatialSynthVoice

    @tags{Audio}
*/
class SpatialSynthSound    : public ReferenceCountedObject
{
protected:
    //==============================================================================
    SpatialSynthSound();

public:
    /** Destructor. */
    ~SpatialSynthSound() override;

    //==============================================================================
    /** Returns true if this sound should be played when a given midi note is pressed.

        The SpatialSynth will use this information when deciding which sounds to trigger
        for a given note.
    */
    virtual bool appliesToNote (int midiNoteNumber) = 0;


    /** The class is reference-counted, so this is a handy pointer class for it. */
    using Ptr = ReferenceCountedObjectPtr<SpatialSynthSound>;


private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (SpatialSynthSound)
};

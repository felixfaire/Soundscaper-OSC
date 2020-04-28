[![N|Solid](https://s3.eu-west-2.amazonaws.com/www.synaesthete.studio/soundscaper-osc/images/SoundscaperOSC_Pages.png)](https://felixfaire.com)

# Soundscaper OSC 

This tool aims to provide an easy way to spatially play sounds through arbitrary numbers of physical speakers in realtime for simple immersive audio installations.

The software allows you to configure:

  - The number of speakers you have connected and their physical 3D co-ordinates.
  - The sound files you want to be playable in that space.
  - Your audio device and network settings.
  
  With this information you can trigger and move virtual sounds around the space in realtime using OSC messages from your application / programming language of choice (see instructions below). Spatial Soundscaper will handle the panning to the various different speakers and will automatically save your configuration for the next time it is opened (useful for automated installation startup).

  To be clear this software is NOT audio control / sequencing software. The timing, positions, animations and interactions of your soundscapes must be implemented in your own code / application that supports OSC. (Processing, Unity, Unreal, Touchdesigner, Cinder, OF etc). This tool can simply act as the audio engine for your project so you can focus on designing the interactions / sound environment behaviour.


### Features:

  - Trigger one shot audio clips at any virtual position in the room.
  - Dynamically move / animate triggered sounds through space over their duration.
  - Multiple looping background file playback for layering global sound atmospheres (plays through all speakers).
  - Supports most of the basic audio filetypes (.wav .mp3 .ogg etc)
  - 128 one shot voice polyphony.
  - Uses Distance Based Amplitude Panning.
  - Cross platform (Supports Mac, Windows and Linux (though not tested on Linux))

### Why:

  This project comes out of a common difficulty / lack of support in handling multichannel audio in realtime software environments like Unity3D and Unreal Engine which are increasingly used to make physical interactive experiences as well as games. I also wanted to make a tool easy enough for students and artists to pick up quickly (Without having to learn MaxMSP / PureData which enable similar functionality) and be robust and performant enough to be used in permanent installations.

### Download:

  Mac and Windows prebuilt binaries coming soon..

## Instructions:

  The application features 3 pages to build your soundscape environment:
  
  - **Space:** to configure your physical speaker positions and visualise playing sounds.
  - **Sounds:** to load all the sound files you wish to use in your soundscape.
  - **Settings:** to configure your audio device and OSC settings.

_Tip: You can also make the application window larger to see all the pages at once._
[![N|Solid](https://s3.eu-west-2.amazonaws.com/www.synaesthete.studio/soundscaper-osc/images/SoundscaperOSC_Main1.png)](https://felixfaire.com)

## Setup:

  1. On the '**Space**' page drag the numbered nodes (each represents a speaker) to match the physical speakers in your room, each grid unit is 1m. You can also click on the node to directly type in 3D coordinates (y is height in space). You can change the plane / section of the space or toggle the list view to insert speaker positions numerically. 

  2. On the **Sounds** page:
     - Select a folder location for your 'atmospheres' (these are looping background audio files that play through all speakers at the same time).
     - Select a folder location for your 'voices' (these one-shot sounds have a 3D position and can be moved in space over their duration).

  3. On the **Settings** page:
     - Make sure your multichannel audio device is present and selected for output.
     - Tick all the channels you wish to be able to use.
     - Make sure the OSC port matches the one coming from your control application.

## OSC Control:
    
### Controlling 'Atmospheres':
  
  All files in the specified atmosphere folder will constantly loop in the background. Their volumes (0.0 - 1.0) can be controlled with the following OSC message format:

    OSC address: 'atmosphere/'
    Arguments:   int: <index-of-atmosphere-file>
               float: <volume>

### Triggering 'Voices':

Simply trigger a sound from the 'voices' list at a certain 3D position with this osc message format:

    OSC address: 'start/'
    Arguments: string: <name-of-voice-file>  (or int: <index-of-voice-file>)
                float: <volume> 
                float: <x>
                float: <y> 
                float: <z>

(Note: _name-of-voice-file_ does not include the file extension)

### Moving 'Voices':
  
Optionally you can also trigger a voice with a unique integer 'voice-id' to be able to move that voice around in space after it has been triggered (see below). Think of this like a unique handle

    OSC address: 'start/'
    Arguments: string: <name-of-voice-file>  (or int: <index-of-voice-file>)
                  int: <voice-id> 
                float: <volume> 
                float: <x>
                float: <y> 
                float: <z>

This identifier allows you to update that sound's position for its duration with the following OSC message format:
  
    OSC address: 'update/'
    Arguments: string: <name-of-voice-file>  (or int: <index-of-voice-file>)
                  int: <voice-id> 
                float: <volume> 
                float: <x>
                float: <y> 
                float: <z>

### Examples

Some practical examples of the above written in [Processing](https://processing.org/) can be found here.
TODO: add link to processing examples on github.

### Current Limitations:

  The software currently uses a method called Distance Based Amplitude Panning (DBAP) which has the advantage of making simple but effective spatial sound fields from arbitrary speaker placements. This method is great for physical installations as it does not require knowledge of the users position (and works for multiple users). However, virtual sounds are only panned effectively if they are played from within convex hull (bounds) of the physical speakers. (There are multiple solutions to this to handle sources that are further away though none are implemented yet).


### Feature Wishlist:

  - Use distance attenuation for sounds outside the convex hull of speakers
  - Allow looping voices
  - Doppler shift with velocity?
  - Ambisonic file decoding and playback.
  - Audio input routing to a virtual 3D voice.
  - ...

### Build Instructions:

  This project relies on the JUCE framework and glm library to build. These libraries should be placed at ../../JUCE and ../../glm or their locations can be respecified in the Projucer project (see JUCE docs for more info on using JUCE).



TODO:   
- Replace osc message listbox with helpful console error output


MIT License

Copyright (c) 2020 Felix Faire

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

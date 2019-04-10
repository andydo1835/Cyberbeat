# Description

    My project will be an interactive music visualizer. It will take as an input an audio file, split it into a few distinct frequencies, and as in a rhythm game such as Dance Dance Revolution,
    make notes appear on the screen according to how pronounced certain frequencies in the audio are. The player will then have to use their keyboard to hit the notes as they appear, in tune
    with the song/audio.

# Outside Library

    I plan to use the ofSound library from openFrameworks to split the audio into frequencies and play sound. I'll also be using the basic openFrameworks libraries for the GUI.

# Background

    I've researched using a Fast Fourier Transform to split audio into frequencies, and read the [provided ofSound introduction](https://openframeworks.cc/ofBook/chapters/sound.html).

# Extensions

    Time permitting, I'd also like to implement the following additional features:
    * Allow the player to set up their own personal keybinds for gameplay
    * Allow the player to input a youtube link instead of a raw audio file
    * Allow the player to adjust difficulty of gameplay
    * Implement a scoring and high score system
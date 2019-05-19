# Cyberbeat
## A self-generating rhythm game using audio onset detection.

This is a simple vertical scrolling rhythm game (in a style similar to StepMania or Dance Dance Revolution with cascading notes) that automatically generates playable maps from audio files. This makes heavy use of the [openFrameworks library](https://openframeworks.cc/).

## Dependencies

* [The openFrameworks library](https://openframeworks.cc/)
* ofxGui, a default addon for openFrameworks
* [ofxSoundObjects, an additional addon for openFrameworks](https://github.com/roymacdonald/ofxSoundObjects)
  * The ofxSoundObjects addon depends on [ofxAudioFile](https://github.com/npisanti/ofxAudioFile) and [ofxFFT](https://github.com/kylemcdonald/ofxFft), although ofxFFT is optional as this project does not use FFT.

## Gameplay

![Screenshot of Gameplay](https://i.imgur.com/Z9u7aBE.png)

The game currently supports 4 keys, mapped to w, e, i, o from left to right, and p to pause the game or change gameplay settings like scroll speed or difficulty. Scroll speed can be adjusted from 5 to 30, with 30 being the fastest, and difficulty can be adjusted from 1.0 to 5.0, with 5.0 being the hardest. The player should press the respective key when a note is over the outlined box.

For each note hit correctly (timing does not matter as long as the note is over the key when the player presses the key), combo is increased by 1, and score is increased by the combo. When a note hits the top of the screen or the player hits a key when there is not a note over it, the game registers a miss, and combo is reset back to 0. The player does not lose any score, so score is always a positive integer. Additionally, for every successful hit, the outlined box will flash its respective color to denote a hit, or when the player misses, the fading text "MISS" will flash in red in the center of the screen to denote a miss.

The game has been timed so that each audio onset detection should relatively match up with when the player should hit the key to match the song. This gives the impression that the player is hitting keys to the beat of the song. When the song is over, the program will exit automatically.

## Usage

Build the project from Visual Studio with [these build configurations](https://i.imgur.com/vxf3pg3.png), then it can be ran straight from the Visual Studio IDE, or an executable will be available in the \bin folder of the repository. The game will initially prompt for an audio file for gameplay. Only .wav, .mp3, FLAC and Ogg Vorbis files are allowed, per the ofxAudioFile library.

## Extensions

For the future, the following features may be added:

* A high scores list to track scores for each song
* Receive audio input from a microphone, or retrieve audio files from online, such as from youtube
* Allowing the user to adjust number of keys for gameplay

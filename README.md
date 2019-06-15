# BDMatch
[中文版]()
An automatic ASS subtitle shifter from a video to another based on audio spectrum.
## Purpose
The shifter can match a subtitle file synced to one video file to another video files. A typical example is TV and BD releases, from which the project name comes.

The purpose of this project is to save the effort of manual syncing. Generally, it provides better accuracy than manual shifting. It attempts to match subtitles by finding similarities in audio streams. With the utilazation of instrinsics and multithreading, the running time of a TV and BD releases of 24 minnutes is generally shorter than 15 seconds, including decoding and matching.

## Downloads
The latest Windows binary release can be found in the [Releases](https://github.com/Thomasyse/BDMatch/releases) section.

## How it works
Two video or audio files and a ass subtitle file that matches one of those files are needed. For the video files, the first audio stream is loaded. With FFmpeg, different video and audio formats are supported.

For every timeline of the subtitles, the program will extract corresponding audio spetrum from the source audio stream and match the most similar part in the destination audio stream, obtaining a shift value which is later applied to the output subtitles.

## Usage
For Windows users, the executable in [Releases](https://github.com/Thomasyse/BDMatch/releases) provides graphic interface.

For users of other platforms, [BDMatchCore](https://github.com/Thomasyse/BDMatch/tree/master/BDMatchCore), including the core functions of the project, can be modified and compiled into a command-line executable.

## Requirements

### Windows releases:
1. Windows 7 or newer
2. CPU with AVX Instrinsics
3. .Net Framework 4.5 or newer

### BDMatchCore Compilation:
1. FFmpeg shared and dev builds
2. fftw shared and dev builds
3. CPU with AVX Instrinsics

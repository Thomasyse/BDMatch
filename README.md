# BDMatch
* [中文版](/README.CHS.md)

An automatic timeline shifter to match a subtitle from one video to another based on audio spectrum comparison.

## Purpose
The shifter can match a subtitle synced to one video to another video. A typical scenario is TV and BD releases for the same episode, from which the project name comes.

The purpose of this project is to save the effort of manual timeline shifting. Generally, it provides better accuracy than manual shifting. It attempts to match subtitles by finding similarities in audio streams. The application of SIMD instruction set and multi-threading allows it to typically process 24 minutes of TV and BD within 10 seconds, including decoding and matching.

## Downloads
The latest Windows binary release can be found in the [`Releases`](https://github.com/Thomasyse/BDMatch/releases) section.

## How it works
Two video or audio files and one subtitle file (ASS/SRT) that synced to one of them are needed. For the video files, the first audio stream is loaded. With FFmpeg, different video and audio formats are supported.

For each timeline of the subtitles, the program will extract the corresponding audio spetrum from the source audio stream and match the most similar part of the target audio stream to obtain the offset that should be applied to the output subtitles.

## Usage
For Windows users, the executable in [`Releases`](https://github.com/Thomasyse/BDMatch/releases) provides graphic interface.

For users of other platforms, [`BDMatchCore`](/BDMatchCore), including the core functionality of the project, can be modified and compiled into a command-line executable. A [`makefile`](/BDMatchCore/makefile) and an [`example of CLI usage`](/BDMatchCore/CLI.cpp) are provided for reference.

## Requirements
* X64 Platform (ARM is not supported currently)

### Windows Releases:
#### BDMatch 1.x (No English Support)
1. Windows 7 or newer
2. CPU with AVX instruction set
3. .Net Framework 4.5 or newer
4. Microsoft Visual C++ Redistributable for Visual Studio 2019
#### BDMatch 2.x
1. Windows 10 1809 (17763) or newer
2. CPU with AVX instruction set
3. .Net 8 or newer (Release with .Net runtime is available in [`Releases`](https://github.com/Thomasyse/BDMatch/releases))
4. [Windows App SDK](https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/downloads) 1.7 or newer (Releases from 2.0.1 onwards include this runtime by default)
5. Microsoft Visual C++ Redistributable for Visual Studio 2022

### BDMatchCore Compilation:
1. FFmpeg shared and dev builds
2. FFTW shared and dev builds

# BDMatch
* [English Version](/README.md)

基于音频频谱的将字幕由一个视频匹配到另一个视频的自动时间轴平移工具。

## 功能
移轴器可以将同步到一个视频文件的字幕文件与另一个视频文件相匹配。典型场景如视频的 TV 和 BD 版本。这也是项目名称的来源。

该项目的目的是节省手动移轴的工作量，它通常能够提供比手动移轴更高的精度。它试图在音频流中寻找相似的部分用以匹配字幕。指令集和多线程的应用将处理24分钟的 TV 和 BD 控制在通常15秒之内（包括解码和匹配）。

## 下载
最新的 Windows 二进制版本可以在 [Releases](https://github.com/Thomasyse/BDMatch/releases) 部分找到。

## 原理
需要提供两个视频或音频文件以及与其中一个文件匹配的字幕文件（ASS/SRT）。对于视频文件，将加载第一个音频流。该项目使用 FFmpeg 以支持不同的视频和音频格式。

对于字幕的每一个时间轴，程序将提取来自源音频流的相应音频谱，并匹配目标音频流中最相似的部分，从而获得稍后应用于输出字幕的偏移量。

## 用法
对于 Windows 用户，[Releases](https://github.com/Thomasyse/BDMatch/releases) 中的可执行文件提供图形界面。

对于其他平台的用户，可以修改 [BDMatchCore](/BDMatchCore)（包括项目的核心功能）并将其编译为命令行可执行文件。一个 [makefile](/BDMatchCore/makefile) 和一个 [CLI示例](/BDMatchCore/CLI.cpp) 可供参考。

## 要求

### Windows 发行版：
1. Windows 7 或更高版本
2. 带 AVX 指令集的 CPU
3. .Net Framework 4.5 或更高版本
4. Microsoft Visual C++ Redistributable for Visual Studio 2019

### BDMatchCore 编译：
1. FFmpeg 共享和开发构建
2. fftw 共享和开发构建
3. 带 AVX 指令集的 CPU

# BDMatch
* [English Version](/README.md)

基于音频频谱对比的将字幕由一个视频匹配到另一个视频的自动时间轴平移工具。

## 功能
该工具可以将基于一个视频的字幕文件与另一个视频相匹配。典型场景如视频的 TV 和 BD 版本。这也是项目名称的来源。

该项目的目的是节省手动移轴的工作量，它通常能够提供比手动移轴更高的精度。它试图在音频流中寻找相似的部分用以匹配字幕。对于SIMD指令集和多线程的应用使其通常将处理24分钟的 TV 和 BD 控制在10秒之内（包括解码和匹配）。

## 下载
最新的 Windows 二进制发行可以在 [`Releases`](/releases) 部分找到。

## 原理
需要提供两个视频或音频文件以及与其中一个文件匹配的字幕文件（ASS/SRT）。对于视频文件，将加载第一个音频流。该项目使用 FFmpeg 以支持不同的视频和音频格式。

对于字幕的每一个时间轴，程序将提取来自源音频流的相应音频谱，并匹配目标音频流中最相似的部分，从而获得应用于输出字幕的偏移量。

## 用法
对于 Windows 用户，[`Releases`](/releases) 中的可执行文件提供图形界面。

对于其他平台的用户，可以修改 [`BDMatchCore`](/BDMatchCore)（包括项目的核心功能）并将其编译为命令行可执行文件。有一个 [`makefile`](/BDMatchCore/makefile) 和一个 [`CLI示例`](/BDMatchCore/CLI.cpp) 可供参考。

## 要求
* X64平台（目前不支持ARM平台）

### Windows 发行版：
#### BDMatch 1.x （无英文支持）
1. Windows 7 或更高版本
2. 带 AVX 指令集的 CPU
3. .Net Framework 4.5 或更高版本
4. Microsoft Visual C++ Redistributable for Visual Studio 2019
#### BDMatch 2.x
1. Windows 10 1809 (17763) 或更高版本
2. 带 AVX 指令集的 CPU
3. .Net 8 或更高版本（[`Releases`](/releases) 中提供带.Net运行时的发行版）
4. Microsoft Visual C++ Redistributable for Visual Studio 2022

### BDMatchCore 编译：
1. FFmpeg 共享和开发构建
2. FFTW 共享和开发构建

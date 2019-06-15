# BDMatch
* [English Version](https://github.com/Thomasyse/BDMatch/blob/master/README.md)

基于音频频谱的将ASS字幕由一个视频匹配到另一个视频的自动ASS字幕时间轴平移工具。

## 功能
移轴器可以将同步到一个视频文件的字幕文件与另一个视频文件相匹配。典型场景如视频的TV和BD版本。这也是项目名称的来源。

该项目的目的是节省手动移轴的工作量。该项目通常能够提供比手动移轴更高的精度。它试图在音频流中寻找相似的部分用以匹配字幕。指令集和多线程的使用使得处理24分钟的TV和BD通常只要不到15秒（包括解码和匹配）。

## 下载
最新的Windows二进制版本可以在[Releases](https://github.com/Thomasyse/BDMatch/releases)部分找到。

## 原理
需要两个视频或音频文件以及与其中一个文件匹配的ass字幕文件。对于视频文件，将加载第一个音频流。该项目使用FFmpeg以支持不同的视频和音频格式。

对于字幕的每一个时间轴，程序将提取来自源音频流的相应音频谱，并匹配目标音频流中最相似的部分，从而获得稍后应用于输出字幕的偏移量。

## 用法
对于Windows用户，[Releases](https://github.com/Thomasyse/BDMatch/releases)中的可执行文件提供图形界面。

对于其他平台的用户，可以修改[BDMatchCore](https://github.com/Thomasyse/BDMatch/tree/master/BDMatchCore)（包括项目的核心功能）并将其编译为命令行可执行文件。

## 要求

### Windows发行版：
1. Windows 7或更高版本
2. 带AVX指令集的CPU
3. .Net Framework 4.5或更高版本

### BDMatchCore编译：
1. FFmpeg共享和开发构建
2. fftw共享和开发构建
3. 带AVX指令集的CPU

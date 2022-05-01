# Cross-Platform Vst Host

## General Idea
Since I found that there are not many easy-to-use command-line based VST Host, and some of them are no longer supported,
 I always wanted to create my own cmd VST Host. Unfortunately, I also found that samples provided in VST 3 SDK are way 
to complicated for me to use, so I decided to created my own easy-to-use repository. Thus, the main goal of this project
 is to learn how to work with VST 3 SDK, how to create custom VST Host and handler VST 3 Plugins. During the work on 
cmd-based VST Host I found out that it would be also a good idea to learn how to read audio from audio endpoint on 
different operation systems like Windows, Linux, MacOS and Android and process audio in real time with VST Host. So that 
is why this repository was created. I know that it will take a lot of work but it will also be a good way to learn about
 everything that I'm interested in. My idea here was also to use as many available elements as it is possible.

## Features list
1. Platform Agnostic Features
    - Implemented:
      - [x] CMake based project
      - [x] Use submodules
      
1. Windows OS

    - Implemented:
      - [x] VST Host library in C++
      - [x] VST Host C++ Offline Tool
      - [x] Unit Test for library and offline tool
      - [x] CI
      - [x] VST Host Library with C api that allows to use it in e.g. Python
      - [x] Basic python unit test for c api
      - [x] Basic code for Audio Endpoint Reader
      - [x] Arg Parser
      - [X] Json Reader 
    - TODO:
      - [ ] Clean up code in vst host lib
      - [ ] Allow to use multiple plugins and configs for them
      - [ ] Clean up audio endpoint reader and utilize it in code (add queue, use it for processing with vst plugin etc)
      - [ ] Add more advanced python-based Vst Host Lib utilization
      - [ ] Clean up UT for python
      - [ ] Integrate better wave reader

1. Linux

    - TODO:
      - [ ] Add build for basic code 
      - [ ] Add endpoint reader
      - [ ] Enable UT
      - [ ] Add python UT
      - [ ] Add platform specific offline tool
      - [ ] Add CI

1. Android

    - TODO:
      - [ ] Add build for basic code 
      - [ ] Add endpoint reader
      - [ ] Enable UT
      - [ ] Add platform specific offline tool
      - [ ] Add CI

1. Mac OS

    - TODO:
      - [ ] Add build for basic code 
      - [ ] Add endpoint reader
      - [ ] Enable UT
      - [ ] Add python UT
      - [ ] Add platform specific offline tool
      - [ ] Add CI


## External Repositories Utilized in Project 
1. [VST 3 SDK](https://github.com/steinbergmedia/vst3sdk)
1. [Google Test](https://github.com/google/googletest)
1. [Logger](https://github.com/amrayn/easyloggingpp)
1. [Wave Reader](https://github.com/audionamix/wave)
1. [C ++ Json Handler](https://github.com/nlohmann/json)
1. [Windows Endpoint Reader](https://github.com/mofo7777/Stackoverflow/tree/master/WasapiCapture)

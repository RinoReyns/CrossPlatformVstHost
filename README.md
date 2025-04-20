[![Coverage Status](https://coveralls.io/repos/github/RinoReyns/CrossPlatformVstHost/badge.png?branch=main)](https://coveralls.io/github/RinoReyns/CrossPlatformVstHost?branch=main)

<p align="left">
    <img alt="ViewCount" src="https://views.whatilearened.today/views/github/CrossPlatformVstHost/github-clone-count-badge.svg">
</p>

[![Build Vst Host](https://github.com/RinoReyns/CrossPlatformVstHost/actions/workflows/vst_host_ci.yml/badge.svg?branch=main)](https://github.com/RinoReyns/CrossPlatformVstHost/actions/workflows/vst_host_ci.yml)

# Cross-Platform Vst Host

## General Idea
Since I found that there are not many easy-to-use command-line based VST Hosts, and some of them are no longer supported,
 I always wanted to create my own cmd VST Host. Unfortunately, I also found that samples provided in VST 3 SDK are way 
to complicated for me to use, so I decided to created my own easy-to-use repository. Thus, the main goal of this project
 is to learn how to work with VST 3 SDK, how to create custom VST Host and handler VST 3 Plugins. During the work on 
cmd-based VST Host I found out that it would be also a good idea to learn how to read audio from audio endpoint on 
different operation systems like Windows, Linux, MacOS and Android and process audio in real time with VST Host. So that 
is why this repository was created. I know that it will take a lot of work but it will also be a good way to learn about
 everything that I'm interested in. My idea here was also to use as many available elements as it is possible.

## How to Build

1. Windows & Linux
    ```
    git submodule update --init --recursive
    pip install -r VstHost_Python/requirements.txt
    mkdir build
    cd build
    cmake ../VstHost_VisualC++ -DCMAKE_BUILD_TYPE=Release -DBUILD_GMOCK=1
    cd ..
    cmake --build build --config Release -j 8
    ```
2. Mac OS (Intel)
    ```
    git submodule update --init --recursive
    pip install -r VstHost_Python/requirements.txt
    brew install doxygen
    mkdir build
    cd build
    cmake -G Xcode ../VstHost_VisualC++ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=gcc -DBUILD_GMOCK=1
    cd ..
    cmake --build build --config Release -j 8 --resolve-package-references on
    ```
3. Mac OS (ARM)

   As a first step you have to install Xcode or/and Command Line Tools for Xcode. If you see following or similar error `XCode 9 or newer is required`, please run command below which should help with the issue based on [stackoverflow thread](https://stackoverflow.com/questions/41380900/cmake-error-no-cmake-c-compiler-could-be-found-using-xcode-and-glfw).
   > sudo xcode-select --reset
   ```
   arch -arm64 brew install cmake
   brew install doxygen
   git submodule update --init --recursive
   pip install -r VstHost_Python/requirements.txt
   mkdir build
   cd build
   cmake ../VstHost_VisualC++ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=gcc -DBUILD_GMOCK=1
   cd ..
   cmake --build build --config Release -j 8 --resolve-package-references on
   ```
4. Android
    ```
    NOTE:
    Following instruction is made for Linux enviroment. However, steps for any OS should be similar.
    1. Download Android NDK and unzip in the chosen location. (https://developer.android.com/ndk/downloads).
    
    2. Set environmental variable for Android NDK as foloow:
       
       > export ANDROID_NDK_PATH=/path/to/android/ndk/main/folder
    
    3. Build VST Host for chosen architecture:
       a. armeabi-v7a:
         > cmake "../VstHost_VisualC++" -B build_armeabi-v7a -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DANDROID_PLATFORM=android-28 -D ANDROID_ABI=armeabi-v7a -DANDROID_BUILD=True
         > cd build_armeabi-v7a
         > $ANDROID_NDK_PATH/prebuilt/linux-x86_64/bin/make
       
       b. arm64-v8a:
         > cmake "../VstHost_VisualC++"  -B build_arm64-v8a -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DANDROID_PLATFORM=android-28 -D ANDROID_ABI=arm64-v8a -DANDROID_BUILD=True
         > cd build_arm64-v8a
         > $ANDROID_NDK_PATH/prebuilt/linux-x86_64/bin/make
       
       c. build_x86:
         > cmake "../VstHost_VisualC++" -B build_x86 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DANDROID_PLATFORM=android-28 -D ANDROID_ABI=x86 -DANDROID_BUILD=True
         > cd build_x86
         > $ANDROID_NDK_PATH/prebuilt/linux-x86_64/bin/make
       
       d. build_x86_64:
         > cmake "../VstHost_VisualC++" -B build_x86_64 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DANDROID_PLATFORM=android-28 -D ANDROID_ABI=x86_64 -DANDROID_BUILD=True
         > cd build_x86_64
         > $ANDROID_NDK_PATH/prebuilt/linux-x86_64/bin/make

    4. Build Android Demo App that uses VST Host lib
       a. Copy necessery dependencess. In the example below dependeces for all architectures were copied. Also note that
          in the example VST plugin has been copied from "build_x86_64" due to the fact that Instrumentation test are 
          executed in androdi simulater on regular CPU.
   
         > mkdir VstHost_Android/AndroidPack
         > cp -rv build_armeabi-v7a/bin/Release/AndroidPack/ VstHost_Android/
         > cp -rv build_arm64-v8a/bin/Release/AndroidPack/ VstHost_Android/
         > cp -rv build_x86/bin/Release/AndroidPack/ VstHost_Android/
         > cp -rv build_x86_64/bin/Release/AndroidPack/ VstHost_Android/
         > cp -rv build_x86_64/VST3/Release VstHost_Android/app/src/main/assets
         > cp -v VstHost_VisualC++/modules/UnitTests/data/sine_440.wav VstHost_Android/app/src/main/assets/
         > cp -v VstHost_VisualC++/modules/UnitTests/data/sine_440_output_ref.wav VstHost_Android/app/src/main/assets/
       
       b. Build Android App
         > cd VstHost_Android
         > chmod +x gradlew
       
       c. Run Intrumentation Test
         > ./gradlew connectedCheck
    ```
## Examples of usage
  1. Print help:
    ```
     VstHostTool.exe --help
    ```
  2. Dump an empty configuration needed to run the tool:
    ```VstHostTool.exe -dump_app_config -config config.json```
  3. Dump an empty configuration for VST plugin/plugins that will be used be.
    ```VstHostTool.exe -dump_plugins_config -config config.json```
  4. Run Audio Processing with Vst Plugin.
    ```VstHostTool.exe -config config.json```

## Features list
1. **Platform Agnostic Features**
    - **Implemented**:
      - [x] CMake based project
      - [x] Use submodules
      - [x] Arg Parser
      - [X] Json Reader 
      - [x] Treat Warnings as Errors
      - [x] VST Host library in C++
      - [x] VST Host Offline Tool in C++
      - [x] Unit Test for library and offline tool in C++
      - [x] CI for each configuration (Windows, Linux, MacOs, Android)
      - [x] VST Host Library with C api that allows to use it in e.g. Python
      - [x] Basic python and android unit test for C api
      - [x] Allow to use multiple plugins and configs for them
      - [x] Generate Documentation
      - [x] AudioProcessing Class that can wrap different non-vst algorithms
      - [x] Cross-Platform Audio Endpoint Render-Capture with RtAudio (Validated only for Windows)
      - [x] Create and pass config for AudioProcessing Class
    - **TODO**:
      - [ ] Implement simple Vst plugin that allows to process data in real-time (e.g. context buffering). Start from 
      delay validation implemented in VST SDK. 
      - [ ] Enable streaming processing (one frame in, one frame out)
      - [ ] Integrate Open Vino to process audio with AI based Audio Algorithms
	  - [ ] Enable Apple Silicon AI acceleration
      - [ ] Add more advanced python-based Vst Host Lib utilization
      - [ ] Add more UT for python and Android
      - [ ] Integrate better wave reader
      - [ ] Build solution for ARM
      - [ ] Handle different audio formats e.g. sampling rate, bit depth etc.
      - [ ] Validate RtAudio for different OSes
      - [ ] Move *vst_host_config_* from *WaveProcessingPipeline* class to *AudioProcessingVstHost* class.
      - [ ] Rename *VstHostTool* to *AudioProcessingTool*
      - [ ] Create Baseclass for processing modules and extract common parameters

1. Android
    - TODO:
      - [ ] Add endpoint reader
      - [ ] Add platform specific offline tool


## Credits (repositories used in project)
1. [VST 3 SDK](https://github.com/steinbergmedia/vst3sdk)
1. [Google Test](https://github.com/google/googletest)
1. [Logger](https://github.com/amrayn/easyloggingpp)
1. [Arg Parser](https://github.com/p-ranav/argparse)
1. [Wave Reader](https://github.com/audionamix/wave)
1. [C ++ Json Handler](https://github.com/nlohmann/json)
1. [Windows Endpoint Reader](https://github.com/mofo7777/Stackoverflow/tree/master/WasapiCapture)
1. [Implementation of audio filters in C](https://github.com/adis300/filter-c)
1. [Real Timie Audio Render and Capture](https://github.com/thestk/rtaudio)

VST is a trademark held by Steinberg Media Technologies, GMBH.

## License
This project is licensed under the terms of the GNU GPLv3 license. Moreover, it is a derivative work of the original 
VST 3 SDK. However, I don't redistribute any of the original source code.

## Useful links
1. [How to read data from audio endpoint (Windows, Linux, MacOS)?](https://github.com/MicrosoftDocs/azure-docs/blob/main/articles/cognitive-services/Speech-Service/how-to-select-audio-input-devices.md)
2. [Audio Capture on Windows](https://graphics.stanford.edu/~mdfisher/Code/Engine/AudioCapture.cpp)
3. [Treat warnings as errors on linux](https://stackoverflow.com/questions/3378560/how-to-disable-gcc-warnings-for-a-few-lines-of-code)
4. [More info about treating warnings as errors](https://www.foonathan.net/2018/10/cmake-warnings/)
5. [How to build c++ code for Android?](https://stackoverflow.com/questions/67729247/build-cmake-library-without-android-studio-by-command-line-or-gui/67729248#67729248)
6. [How to use c++ .so on Android?](https://github.com/android/ndk-samples/tree/master/hello-libs)
7. [Rendering an audio stream on windows](https://docs.microsoft.com/en-us/windows/win32/coreaudio/rendering-a-stream)
8. [XCode Dependencies Installation](https://stackoverflow.com/questions/41380900/cmake-error-no-cmake-c-compiler-could-be-found-using-xcode-and-glfw)

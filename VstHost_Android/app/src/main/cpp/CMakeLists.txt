cmake_minimum_required(VERSION 3.22.1)

set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../VstHost_VisualC++/vst3sdk")
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)
set(VST_SDK TRUE)
set(SMTG_CREATE_PLUGIN_LINK FALSE)
set(SDK_IDE_PLUGIN_EXAMPLES_FOLDER FOLDER "PlugInExamples")

list(APPEND CMAKE_MODULE_PATH "${SDK_ROOT}/cmake/modules")
include(SMTG_VST3_SDK)
include(SMTG_CustomModuleTarget)

smtg_setup_platform_toolset()

include_directories(${SDK_ROOT})
add_subdirectory(${SDK_ROOT}/pluginterfaces build/pluginterfaces)
add_subdirectory(${SDK_ROOT}/base build/base)
add_subdirectory(${SDK_ROOT}/public.sdk  build/public.sdk)

set(package_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../AndroidPack)
add_library(lib_AudioHostLib SHARED IMPORTED)
set_target_properties(lib_AudioHostLib PROPERTIES IMPORTED_LOCATION
                     ${package_dir}/lib/${ANDROID_ABI}/libAudioHostLib.so)

add_library(lib_libWaveIO STATIC IMPORTED)
set_target_properties(lib_libWaveIO PROPERTIES IMPORTED_LOCATION ${package_dir}/lib/${ANDROID_ABI}/libWaveIO.a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library(VstHostLibWrapper SHARED
            VstHostLibWrapper.cpp)

target_include_directories(VstHostLibWrapper PRIVATE
                           ${package_dir}/include
                           ${SDK_ROOT}/public.sdk/source/vst/hosting
                           ${SDK_ROOT}/public.sdk/source/vst/utility
                           ${SDK_ROOT}/pluginterfaces/vst
                           )
target_link_libraries(VstHostLibWrapper
                      lib_AudioHostLib
                      sdk
                      sdk_hosting
                      lib_libWaveIO
                      )

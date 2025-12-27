cmake_minimum_required (VERSION 3.11.0)

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk/cmake/modules")
include(cmake/custom_functions.cmake)
include(SMTG_VST3_SDK)
include(SMTG_CustomModuleTarget)
include_directories(${SDK_ROOT})

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.15" CACHE STRING "macOS deployment target")
    message(STATUS "[CrossPlatformVstHost] macOS Deployment Target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

endif()

SET(ANDROID_BUILD False CACHE BOOL "Determin if build is dedicated for andorid.")
if (${ANDROID_BUILD})
    message("-- Android Build: ${ANDROID_BUILD}")
    include(cmake/andorid_toolchain.cmake)

endif()
#-------------------------------------------------------------------------------
# Project
#-------------------------------------------------------------------------------
project(VstHost C CXX)
smtg_setup_platform_toolset()

SET(vstsdk_VERSION 3.7.14)
set(SMTG_CREATE_MODULE_INFO OFF CACHE BOOL "" FORCE)
set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vst3sdk")
set(SDK_IDE_LIBS_FOLDER FOLDER "Libraries")
set(UNIT_TESTS_FOLDER FOLDER "UnitTest")
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)
set(VST_SDK TRUE)
set(SMTG_CREATE_PLUGIN_LINK FALSE)
set(SDK_IDE_PLUGIN_EXAMPLES_FOLDER FOLDER "PlugInExamples")
set(SUFFIX_CONFIG_TYPE $<IF:$<CONFIG:Debug>,Debug,Release>)

if (MSVC)
    set(SMTG_USE_STDATOMIC_H OFF CACHE BOOL "" FORCE)
    set(AUDIO_ENDPOINTS_FOLDER FOLDER "AudioEndpoints")
endif()

message(STATUS "CMAKE_SOURCE_DIR is set to: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_DIR is set to: ${CMAKE_CURRENT_LIST_DIR}")
add_subdirectory(vst3sdk/pluginterfaces)
add_subdirectory(vst3sdk/base)
add_subdirectory(vst3sdk/public.sdk)
add_subdirectory(vst3sdk/public.sdk/samples/vst/adelay)
add_subdirectory(modules)
add_subdirectory(external_modules)

if (NOT ${ANDROID_BUILD})
    set_target_properties(gtest PROPERTIES ${UNIT_TESTS_FOLDER})
    set_target_properties(gtest_main PROPERTIES ${UNIT_TESTS_FOLDER})
    set_target_properties(gmock PROPERTIES ${UNIT_TESTS_FOLDER})
    set_target_properties(gmock_main PROPERTIES ${UNIT_TESTS_FOLDER})
    set_target_properties(ApiUnitTests PROPERTIES ${UNIT_TESTS_FOLDER})
    set_target_properties(OfflineToolsUnitTests PROPERTIES ${UNIT_TESTS_FOLDER})   
endif()

set_target_properties(sdk PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(sdk_common PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(sdk_hosting PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(base PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(pluginterfaces PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(cmake_modules PROPERTIES ${SDK_IDE_LIBS_FOLDER})

if (MSVC)
    set_target_properties(AudioEndpointManager PROPERTIES ${AUDIO_ENDPOINTS_FOLDER})
    set_target_properties(AudioRender PROPERTIES ${AUDIO_ENDPOINTS_FOLDER})
    set_target_properties(AudioCapture PROPERTIES ${AUDIO_ENDPOINTS_FOLDER})
    set_target_properties(AudioEndpointBase PROPERTIES ${AUDIO_ENDPOINTS_FOLDER})
endif()

if(TARGET base_ios)
    set_target_properties(base_ios PROPERTIES ${SDK_IDE_LIBS_FOLDER})
    set_target_properties(pluginterfaces_ios PROPERTIES ${SDK_IDE_LIBS_FOLDER})
endif(TARGET base_ios)

option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Docs)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( Documentation ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
        
    add_custom_command(TARGET Documentation POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Documentation
                                                                   ${CMAKE_CURRENT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/Documentation
                       COMMAND cmake -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/../Documentation
    )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

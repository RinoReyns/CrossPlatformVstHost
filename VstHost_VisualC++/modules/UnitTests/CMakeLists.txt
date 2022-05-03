set(target ApiUnitTests)
set(target_offline_tools OfflineToolsUnitTests)

project(${target})
add_subdirectory(./googletest)

enable_testing()

include_directories(${gtest_SOURCE_DIR}/include 
                    ${gtest_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/header
                    SYSTEM ${GTEST_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/../VstHostTool/header)

set(UnitTests_sources
    header/Common.h
    src/AudioHostLibUnitTests.cpp)

set (OfflineToolsUnitTest
     header/Common.h
     src/VstHostToolUnitTests.cpp)

add_executable(${target} ${UnitTests_sources})
target_link_libraries(${target} gtest gtest_main AudioHostLib WaveIO Logger Common ArgParser sdk_hosting base sdk)
add_test(NAME ${target} COMMAND ${target})

add_executable(${target_offline_tools} ${OfflineToolsUnitTest})
target_link_libraries(${target_offline_tools} gtest gtest_main VstHostToolLib WaveIO Logger Common ArgParser base sdk)
add_test(NAME ${target_offline_tools} COMMAND ${target_offline_tools})

set(SUFFIX_CONFIG_TYPE $<IF:$<CONFIG:Debug>,Debug,Release>)


if(SMTG_WIN)
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3/Contents/x86_64-win/adelay.vst3 
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data                                 
    )

    add_custom_command(TARGET ${target_offline_tools} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3/Contents/x86_64-win/adelay.vst3 
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3/Contents/x86_64-win/adelay.vst3 
                                                         ${PROJECT_BINARY_DIR}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data                                 
    )

elseif(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3 
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../lib/${SUFFIX_CONFIG_TYPE}/libAudioHostLib.so
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/                                          
    )

    add_custom_command(TARGET ${target_offline_tools} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3 
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3 
                                                         ${PROJECT_BINARY_DIR}/adelay.vst3 
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data    
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../lib/${SUFFIX_CONFIG_TYPE}/libAudioHostLib.so
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/                        
    )
else()
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/adelay.vst3
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../lib/${SUFFIX_CONFIG_TYPE}/libAudioHostLib.dylib
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/
    )

    add_custom_command(TARGET ${target_offline_tools} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3
                                                 ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/adelay.vst3
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/../../VST3/${SUFFIX_CONFIG_TYPE}/adelay.vst3
                                                         ${PROJECT_BINARY_DIR}/adelay.vst3
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/data
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data
                                                        ${PROJECT_BINARY_DIR}/data
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../../lib/${SUFFIX_CONFIG_TYPE}/libAudioHostLib.dylib
                                                        ${PROJECT_BINARY_DIR}/../../bin/${SUFFIX_CONFIG_TYPE}/
    )

endif(SMTG_WIN)

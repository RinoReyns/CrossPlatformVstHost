set(audiohost_sources
    ${SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.cpp
    ${SDK_ROOT}/public.sdk/source/vst/hosting/plugprovider.h
    source/audiohost.cpp
    header/audiohost.h
)

if(SMTG_MAC)
    set(audiohost_sources 
        ${audiohost_sources}
        ${SDK_ROOT}/public.sdk/source/vst/hosting/module_mac.mm
    )
    set(audiohost_PLATFORM_LIBS "-framework Cocoa")
    get_filename_component(InfoPlistFile "resource/Info.plist" ABSOLUTE)
    set(APP_PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile}
    )
    set_property (TARGET ${target} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
elseif(SMTG_LINUX)
    set(audiohost_sources
        ${audiohost_sources}
        ${SDK_ROOT}/public.sdk/source/vst/hosting/module_linux.cpp
    )
    set(audiohost_PLATFORM_LIBS)
    set(APP_PROPERTIES)
elseif(SMTG_WIN)
    set(audiohost_sources
        ${audiohost_sources}
        ${SDK_ROOT}/public.sdk/source/vst/hosting/module_win32.cpp
    )
    set(audiohost_PLATFORM_LIBS)
endif(SMTG_MAC)

set(target AudioHostLib)

include_directories(include ${PROJECT_BINARY_DIR})
add_library(${target} SHARED ${audiohost_sources})
add_compile_definitions(${target} PUBLIC ${target})

target_compile_features(${target} PUBLIC cxx_std_17)

if(SMTG_MAC)
    target_compile_options(${target} PUBLIC "-fobjc-arc")
endif()

target_include_directories(${target}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header 
                                  ${CMAKE_CURRENT_SOURCE_DIR}/source/media
                                  ${SDK_ROOT}/public.sdk/source/vst/hosting
                                  ${SDK_ROOT}/public.sdk/source/vst/utility
                                  ${SDK_ROOT}/pluginterfaces/vst
                                  ${PROJECT_BINARY_DIR}/modules/VstHost
                                  ${PROJECT_SOURCE_DIR})
                 
target_link_libraries(${target} PRIVATE base sdk WaveIO Logger Common sdk_hosting)

include (GenerateExportHeader)
generate_export_header(AudioHostLib)
if (${ANDROID_BUILD})
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libAudioHostLib.so
                                                        ${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libAudioHostLib.so
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/header/audiohost.h
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/audiohost.h"
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/modules/VstHost/audiohostlib_export.h
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/audiohostlib_export.h"
        )
        
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libpluginterfaces.a
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libpluginterfaces.a"
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libsdk.a
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libsdk.a"
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libsdk_hosting.a
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libsdk_hosting.a"
        )
endif()

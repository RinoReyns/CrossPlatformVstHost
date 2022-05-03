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
    set_property (TARGET ${target} APPEND_STRING PROPERTY 
              COMPILE_FLAGS "-fobjc-arc")
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
target_compile_options(${target} PUBLIC "-fobjc-arc")
target_include_directories(${target} 
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header 
                                  ${CMAKE_CURRENT_SOURCE_DIR}/source/media
                                  ${SDK_ROOT}/public.sdk/source/vst/hosting
                                  ${SDK_ROOT}/pluginterfaces/vst
                                  ${SDK_ROOT}/public.sdk/source/vst/utility/
                                  ${PROJECT_BINARY_DIR}/modules/VstHost
                                  ${PROJECT_SOURCE_DIR})
                 
target_link_libraries(${target} PRIVATE base sdk ArgParser WaveIO Logger Common sdk_hosting)

include (GenerateExportHeader)
generate_export_header(AudioHostLib)


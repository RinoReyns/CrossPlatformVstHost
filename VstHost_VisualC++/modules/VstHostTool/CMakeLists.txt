if (NOT ${ANDROID_BUILD})
    project (VstHostTool)
    set(target VstHostTool)

    include_directories(${CMAKE_SOURCE_DIR}/modules/VstHost/header 
                        ${PROJECT_BINARY_DIR}/../VstHost
                        ${SDK_ROOT}/pluginterfaces/vst
                        ${SDK_ROOT}/public.sdk/source/vst/hosting
                        ${CMAKE_CURRENT_SOURCE_DIR}/header)

    set(LIB_SOURCE
        src/VstHostTool.cpp
        header/VstHostTool.h
        )

    set(SOURCES
        src/main.cpp
    )

    add_library(${target}Lib ${LIB_SOURCE})
    add_executable(${target} ${SOURCES})

    if(SMTG_WIN)
        target_link_libraries (${target}Lib PUBLIC AudioHostLib
                               PRIVATE base sdk ArgParser Logger Common sdk_hosting AudioEndpointManager AudioCapture AudioRender AudioEndpointBase)
        target_link_libraries (${target} PUBLIC ${target}Lib
                               PRIVATE base sdk ArgParser Logger Common sdk_hosting AudioEndpointManager AudioCapture AudioRender AudioEndpointBase)
    else()

        target_link_libraries (${target}Lib PUBLIC AudioHostLib
                               PRIVATE base sdk ArgParser Logger Common sdk_hosting)
        target_link_libraries (${target} PUBLIC ${target}Lib
                               PRIVATE base sdk ArgParser Logger Common)

    endif(SMTG_WIN)
endif()              

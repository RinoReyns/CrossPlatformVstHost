if (NOT ${ANDROID_BUILD})
    project (VstHostTool)
    set(target VstHostTool)
    set(${target}Lib ${target}Lib )
  
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
        target_link_libraries (${target}Lib PUBLIC WaveIO AudioProcessing AudioHostLib
                               PRIVATE base sdk ArgParser Logger Common AudioEndpointManager AudioCapture AudioRender AudioEndpointBase)
        target_include_directories(${target}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
        
        target_link_libraries (${target}
                               PRIVATE ${target}Lib base sdk ArgParser Logger Common)
    else()

        target_link_libraries (${target}Lib PUBLIC WaveIO AudioProcessing AudioHostLib
                               PRIVATE base sdk ArgParser Logger Common sdk_hosting)
        target_include_directories(${target}Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
        target_link_libraries (${target} PUBLIC ${target}Lib
                               PRIVATE base sdk ArgParser Logger Common)

    endif(SMTG_WIN)
endif()              

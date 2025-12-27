if(SMTG_LINUX)
    message("[WARNING] Audio Endpoint Manager for Linux is not implemented.")
elseif(SMTG_WIN)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../external_modules/RtAudio/rtaudio/)

    set(target AudioEndpointManager)
    set(audio_enpoint_manager_headers
        header/AudioEndpointManager.h
        )
    set(audio_enpoint_manager_src
        source/AudioEndpointManager.cpp
        )
   add_library(${target} ${audio_enpoint_manager_src} ${audio_enpoint_manager_headers})
   target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
   target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../external_modules/RtAudio/rtaudio/)
   target_compile_features(${target} PUBLIC cxx_std_17)
   target_link_libraries(${target} PRIVATE Common Logger AudioCapture AudioRender rtaudio AudioEndpointBase)

endif(SMTG_LINUX)

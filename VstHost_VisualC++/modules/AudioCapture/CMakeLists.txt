if(SMTG_LINUX)
    message("[WARNING] Audio Capture for Linux is not implemented.")
elseif(SMTG_WIN)
    set(target AudioCapture)

    set(audio_caputre_headers
        header/WaveWriter.h
        header/AudioCapture.h
        )

    set(audio_caputre_src
        source/WaveWriter.cpp
        source/AudioCapture.cpp
        )

    add_library(${target} ${audio_caputre_src} ${audio_caputre_headers})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_link_libraries(${target} PRIVATE Common Logger AudioEndpointBase)
endif(SMTG_LINUX)


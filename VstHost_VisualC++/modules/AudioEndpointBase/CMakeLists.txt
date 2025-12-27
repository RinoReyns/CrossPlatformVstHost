if(SMTG_LINUX)
    message("[WARNING] Audio Endpoint Manager for Linux is not implemented.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
elseif(SMTG_WIN)
    set(target AudioEndpointBase)

    set(audio_enpoint_base_headers
        header/AudioEndpointBase.h
        )

    set(audio_enpoint_base_src
        source/AudioEndpointBase.cpp
        )

    add_library(${target} ${audio_enpoint_base_src} ${audio_enpoint_base_headers})
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_link_libraries(${target} PRIVATE Common Logger)
endif(SMTG_LINUX)

set(target WaveIO)

set(WaveIO_headers
    header/fmt_header.h
    header/header.h
    header/header_list.h
    header/file.h
    header/error.h
    header/WaveIOClass.h
    )

set (WaveIO_src
     src/fmt_header.cc
     src/header.cc
     src/header_list.cc
     src/file.cc
     src/WaveIOClass.cpp
     )

add_library(${target} ${WaveIO_src} ${WaveIO_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target} Common Logger)

if (${ANDROID_BUILD})
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/header/
                                                                  "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/"
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libWaveIO.a
                                                        ${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libWaveIO.a
        )

        
endif()

set(target AudioProcessing)

set(AudioProcessing_headers
    filter-c/filter.h
    )

set (AudioProcessing_src
     filter-c/filter.c
     )

add_library(${target} ${AudioProcessing_src} ${AudioProcessing_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target})

if (${ANDROID_BUILD})
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/header/
                                                                  "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/"
                       COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/lib/${SUFFIX_CONFIG_TYPE}/libAudioProcessing.a
                                                        ${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/lib/${ANDROID_ABI}/libAudioProcessing.a
        )

        
endif()

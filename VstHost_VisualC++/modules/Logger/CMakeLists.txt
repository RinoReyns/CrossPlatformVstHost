set(target Logger)

set(logger_headers
    header/easylogging++.h
    )

set (logger_src
     src/easylogging++.cc
    )

add_compile_definitions(ELPP_DEFAULT_LOG_FILE="VstHost.log") 
add_compile_definitions(ELPP_DEFAULT_LOGGER="VstHostLogger") 

add_library(${target} ${logger_src} ${logger_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target} PRIVATE Common)

if(LINUX)
    set(CMAKE_CXX_FLAGS "-Wno-multichar")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -fno-omit-frame-pointer")
endif()

if (${ANDROID_BUILD})
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/header/easylogging++.h
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/easylogging++.h"
        )
        
endif()

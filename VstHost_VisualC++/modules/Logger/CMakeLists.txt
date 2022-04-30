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

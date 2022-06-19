set(target Common)

set(Common_headers
    header/enums.h
    header/MFMacro.h
    header/StdAfx.h
    json/single_include/nlohmann/json.hpp
    header/JsonUtils.h
    header/Utils.h
    header/VstHostMacro.h
    )

set (Common_src
     src/JsonUtils.cpp
     src/Utils.cpp
     )

add_library(${target} ${Common_src} ${Common_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header 
                           ${CMAKE_CURRENT_SOURCE_DIR}/json/single_include/nlohmann)
target_compile_features(${target} PUBLIC  cxx_std_17)
target_link_libraries(${target} PRIVATE Logger)

if (${ANDROID_BUILD})
    add_custom_command(TARGET ${target} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/header/enums.h
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/enums.h"
                       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/header/VstHostMacro.h
                                                        "${PROJECT_BINARY_DIR}/bin/${SUFFIX_CONFIG_TYPE}/AndroidPack/include/VstHostMacro.h"
        )
endif()

set(target Common)

set(Common_headers
    header/enums.h
    header/MFMacro.h
    header/StdAfx.h
    json/single_include/nlohmann/json.hpp
    header/JsonUtils.h
    )

set (Common_src
     src/JsonUtils.cpp
     )

add_library(${target} ${Common_src} ${Common_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header 
                           ${CMAKE_CURRENT_SOURCE_DIR}/json/single_include/nlohmann)
target_compile_features(${target} PUBLIC  cxx_std_17)
target_link_libraries(${target} PRIVATE Logger)

set(target Common)

set(Common_headers
    header/enums.h
    header/MFMacro.h
    header/StdAfx.h
    json/single_include/nlohmann/json.hpp
    )

add_library(${target} INTERFACE )
target_include_directories(${target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/header 
                           ${CMAKE_CURRENT_SOURCE_DIR}/json/single_include/nlohmann)
target_compile_features(${target} INTERFACE  cxx_std_17)
target_link_libraries(${target})

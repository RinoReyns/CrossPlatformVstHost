set(target ArgParser)

set(arg_parser_headers
    header/argparser.hpp
    header/arg_parser.h
    )

set (arg_parser_src
     src/arg_parser.cpp
    )

add_library(${target} ${arg_parser_src} ${arg_parser_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target} PRIVATE Common Logger)


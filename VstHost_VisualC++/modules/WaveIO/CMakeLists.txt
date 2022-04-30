set(target WaveIO)

set(WaveIO_headers
    header/fmt_header.h
    header/header.h
    header/header_list.h
    header/file.h
    header/error.h
    )

set (WaveIO_src
     src/fmt_header.cc
     src/header.cc
     src/header_list.cc
     src/file.cc
     )

add_library(${target} ${WaveIO_src} ${WaveIO_headers})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target})

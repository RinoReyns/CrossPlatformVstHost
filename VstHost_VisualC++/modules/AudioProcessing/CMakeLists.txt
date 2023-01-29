set(target AudioProcessing)

set(AudioProcessing_headers
    filter-c/filter.h
	header/FilterWrapper.h
    header/WaveProcessingPipeline.h
    )

set (AudioProcessing_src
     filter-c/filter.c
     source/FilterWrapper.cpp
     source/WaveProcessingPipeline.cpp
     )


add_library(${target} ${AudioProcessing_src} ${AudioProcessing_headers})
target_include_directories(${target} 
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/filter-c 
                            ${CMAKE_CURRENT_SOURCE_DIR}/header)
target_compile_features(${target} PUBLIC cxx_std_17)
target_link_libraries(${target} PRIVATE Common Logger AudioHostLib WaveIO)

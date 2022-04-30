if(ANDROID)
    return()
endif(ANDROID)

if (MSVC)
    add_compile_options(/W4 /WX)
    add_link_options(/WX)
endif()

include(SMTG_AddSubDirectories)
smtg_add_subdirectories()

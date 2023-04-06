if (MSVC)
    add_compile_options(/W4 /WX)
    add_link_options(/WX)
elseif (UNIX AND !${ANDROID_BUILD})
    add_compile_options(-Werror -Wextra)
endif()

include(SMTG_AddSubDirectories)
smtg_add_subdirectories()

if (MSVC)
   add_compile_options(/W1 /WX)
   add_link_options(/WX)
   include(SMTG_AddSubDirectories)
   smtg_add_subdirectories()
elseif (UNIX AND !${ANDROID_BUILD})
    add_compile_options(-Werror -Wextra)
elseif(LINUX)
    find_program(GCOV_PATH gcov)
    if(NOT GCOV_PATH)
	  message(${GCOV_PATH})
	  message("[WARNING] Code coverage analysis requires gcov!")
    else()
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")	
	  message("[MESSAGE] Code coverage analysis found gcov!")
    endif()
    include(SMTG_AddSubDirectories)
    smtg_add_subdirectories()
endif()




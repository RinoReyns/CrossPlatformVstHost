if ("$ENV{ANDROID_NDK_PATH}" STREQUAL "")
    message(FATAL_ERROR "Environmental variable 'ANDROID_NDK_PATH' is empty. Please set it and rerun build.")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

message ($ENV{ANDROID_NDK_PATH})
set(CMAKE_TOOLCHAIN_DIRECTORY "$ENV{ANDROID_NDK_PATH}/build/cmake")
if(NOT CMAKE_TOOLCHAIN_FILE)
  if(NOT ANDROID_ABI)
    # Default to 32 Bit ARMv7 CPU.
    set(ANDROID_ABI "armeabi-v7a")
  endif()
  if(NOT ANDROID_PLATFORM)
    set(ANDROID_PLATFORM "android-28")
  endif()
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_DIRECTORY}/android.toolchain.cmake")
endif()

IF (WIN32)
        set(CMAKE_MAKE_PROGRAM "$ENV{ANDROID_NDK_PATH}\\prebuilt\\windows-x86_64\\bin\\make.exe" CACHE INTERNAL "" FORCE)
endif()

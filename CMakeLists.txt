cmake_minimum_required(VERSION 3.13)
project(isometric_maps)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_PATH x11)
    set(OS "linux")
    add_compile_definitions(X11_ENABLED)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TARGET_PATH win64)
    set(OS "windows")
    add_compile_definitions(WINDOWS_ENABLED)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TARGET_PATH macos)
    set(OS "osx")
    add_compile_definitions(OSX_ENABLED)
    add_compile_definitions(APPLE_STYLE_KEYS)
else()
    message(FATAL_ERROR "Not implemented support for ${CMAKE_SYSTEM_NAME}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GODOT_COMPILE_FLAGS )
set(GODOT_LINKER_FLAGS )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /EHsc /WX") # /GF /MP
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /DTYPED_METHOD_BIND")

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /MDd") # /Od /RTC1 /Zi
    else()
        set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /MD /O2") # /Oy /GL /Gy
        STRING(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    endif(CMAKE_BUILD_TYPE MATCHES Debug)

    # Disable conversion warning, truncation, unreferenced var, signed mismatch
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /wd4244 /wd4305 /wd4101 /wd4018 /wd4267")

    add_definitions(-DNOMINMAX)

    # Unkomment for warning level 4
    #if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    #	string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    #endif()

else()

    #elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    #elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC and maybe MinGW?

    set(GODOT_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,-R,'$$ORIGIN'")

    # Hmm.. maybe to strikt?
    set(GODOT_COMPILE_FLAGS "-fPIC -g -Wwrite-strings")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wchar-subscripts -Wcomment -Wdisabled-optimization")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wformat -Wformat=2 -Wformat-security -Wformat-y2k")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wimport -Winit-self -Winline -Winvalid-pch -Werror")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wmissing-braces -Wmissing-format-attribute")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpointer-arith")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wredundant-decls -Wreturn-type -Wsequence-point")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wswitch -Wswitch-enum -Wtrigraphs")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wunused-label")
    set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wunused-value -Wvariadic-macros -Wvolatile-register-var -Wno-error=attributes")

    # -Wshadow -Wextra -Wall -Weffc++ -Wfloat-equal -Wstack-protector -Wunused-parameter -Wsign-compare -Wunused-variable -Wcast-align
    # -Wunused-function -Wstrict-aliasing -Wstrict-aliasing=2 -Wmissing-field-initializers

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -Wno-ignored-attributes")
    endif()

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -fno-omit-frame-pointer -O0")
    else()
        set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -O3")
    endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif()

set(GODOT_ROOT_DIR ../../)

# Uncomment to pass auto completion in TOOLS mode
add_compile_definitions(TOOLS_ENABLED)
add_compile_definitions(DEBUG_ENABLED)
add_definitions(-DUNIX_ENABLED -DVULKAN_ENABLED -DANDROID_ENABLED -DGLES3_ENABLED)
add_definitions(${GODOT_COMPILE_FLAGS}  ${GODOT_LINKER_FLAGS})

# Get sources
file(GLOB_RECURSE SOURCES src/*.c**)
file(GLOB_RECURSE HEADERS src/*.h**)

include_directories(./src/)
include_directories(${GODOT_ROOT_DIR})

add_executable(${PROJECT_NAME} register_types.h register_types.cpp ${SOURCES} ${HEADERS} src/isometric_string_names.h src/isometric_string_names.cpp)
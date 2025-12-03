cmake_minimum_required(VERSION 3.28)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_program(MOLD_PROGRAM mold)
if(MOLD_PROGRAM)
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=mold")
endif()

option(CATHEDRAL_BUILD_TESTS "Build tests" ON)
option(CATHEDRAL_PCH "Enable precompiled headers" ON)
option(CATHEDRAL_NO_CHECKS "Disable runtime sanity checks" OFF)
option(CATHEDRAL_AVX2 "Enable AVX2 instruction set" ON)

if (UNIX)
    option(CATHEDRAL_LINUX_FORCE_X11 "Force use of X11 on Linux OSes" OFF)
    if(CATHEDRAL_LINUX_FORCE_X11)
        add_compile_definitions(CATHEDRAL_LINUX_FORCE_X11)
    endif()
endif ()

project(cathedral LANGUAGES C CXX)

if(CATHEDRAL_SANITIZE_ADDRESS)
    set(SANITIZE_MODE address) # address, thread, leak, undefined,...
    add_compile_options(-fsanitize=${SANITIZE_MODE})
    add_link_options(-fsanitize=${SANITIZE_MODE})
endif()

if(CATHEDRAL_AVX2)
    if(MSVC)
        add_compile_options(/arch:AVX2)
    else()
        add_compile_options(-mavx2)
    endif ()
endif()

include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(resources)
add_subdirectory(third-party)

if (CATHEDRAL_BUILD_TESTS)
    add_subdirectory(tests)
endif ()
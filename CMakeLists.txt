cmake_minimum_required(VERSION 3.28)

#find_program(CCACHE_PROGRAM ccache)
#if(CCACHE_PROGRAM)
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
#    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}") 
#endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_program(MOLD_PROGRAM mold)
if(MOLD_PROGRAM)
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=mold")
endif()

project(cathedral LANGUAGES C CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    set(SANITIZE_MODE address) # address, thread, leak, undefined,...
    add_compile_options(-fsanitize=${SANITIZE_MODE})
    add_link_options(-fsanitize=${SANITIZE_MODE})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(third-party)
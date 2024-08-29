cmake_minimum_required(VERSION 3.28)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}") 
endif()

#find_program(MOLD_PROGRAM mold)
#if(MOLD_PROGRAM)
#    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
#    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=mold")
#endif()

project(cathedral LANGUAGES C CXX)

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_options(-fsanitize=address)
#    add_link_options(-fsanitize=address)
#endif()

include(${CMAKE_SOURCE_DIR}/dependencies.cmake)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(third-party)
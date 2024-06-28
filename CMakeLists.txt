cmake_minimum_required(VERSION 3.28)

project(cathedral LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/dependencies.cmake)

add_subdirectory(bin)
add_subdirectory(lib)
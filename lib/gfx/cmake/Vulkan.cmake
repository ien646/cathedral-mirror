if(WIN32)
    set(LIB_PATH_HINTS
        $ENV{VULKAN_SDK}
        $ENV{VULKAN_SDK}/Lib
    )

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        message(STATUS "Attempting to find DEBUG additional libs...")
        #find_library(LIB_SPIRV_CROSS_CORE NAME "spirv-cross-cored" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_CPP NAME "spirv-cross-cppd" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_GLSL NAME "spirv-cross-glsld" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_REFLECT NAME "spirv-cross-reflectd" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        find_library(LIB_SHADERC_COMBINED NAME "shaderc_combinedd" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    else()
        message(STATUS "Attempting to find RELEASE additional libs...")
        #find_library(LIB_SPIRV_CROSS_CORE NAMES "spirv-cross-core" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_CPP NAMES "spirv-cross-cpp" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_GLSL NAMES "spirv-cross-glsl" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        #find_library(LIB_SPIRV_CROSS_REFLECT NAMES "spirv-cross-reflect" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
        find_library(LIB_SHADERC_COMBINED NAME "shaderc_combined" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH) 
    endif()
else()
    find_package(Vulkan COMPONENTS shaderc_combined REQUIRED)

    set(LIB_PATH_HINTS
        $ENV{VULKAN_SDK}
        $ENV{VULKAN_SDK}/lib
        $ENV{VULKAN_SDK}/*/lib
    )

    set(INCLUDE_PATH_HINTS
        $ENV{VULKAN_SDK}
        $ENV{VULKAN_SDK}/include/*
        $ENV{VULKAN_SDK}/*/include/*
    )
    find_path(SPIRV_CROSS_CORE_INCLUDE_DIR NAMES "spirv_reflect.hpp" HINTS ${INCLUDE_PATH_HINTS} REQUIRED)
    cmake_path(GET SPIRV_CROSS_CORE_INCLUDE_DIR PARENT_PATH VULKAN_INCLUDE_DIR)

    #find_library(LIB_SPIRV_CROSS_CORE NAMES "spirv-cross-core" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    #find_library(LIB_SPIRV_CROSS_CPP NAMES "spirv-cross-cpp" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    #find_library(LIB_SPIRV_CROSS_GLSL NAMES "spirv-cross-glsl" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    #find_library(LIB_SPIRV_CROSS_REFLECT NAMES "spirv-cross-reflect" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    find_library(LIB_SHADERC_COMBINED NAME "shaderc_combined" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
endif()

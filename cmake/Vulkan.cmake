if(WIN32)
    set(LIB_PATH_HINTS
        $ENV{VULKAN_SDK}
        $ENV{VULKAN_SDK}/Lib
    )

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        find_library(LIB_SHADERC_COMBINED NAME "shaderc_combinedd" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
    else()
        find_library(LIB_SHADERC_COMBINED NAME "shaderc_combined" HINTS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH) 
    endif()
else()
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

    find_library(LIB_SHADERC_COMBINED NAME "shaderc_combined" PATHS ${LIB_PATH_HINTS} REQUIRED NO_DEFAULT_PATH)
endif()

find_package(Vulkan COMPONENTS shaderc_combined REQUIRED)
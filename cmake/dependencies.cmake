set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cache)
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

include(${CMAKE_SOURCE_DIR}/cmake/Vulkan.cmake)

find_package(OpenMP REQUIRED)

CPMAddPackage(
        NAME Boost
        VERSION 1.86.0 # Versions less than 1.85.0 may need patches for installation targets.
        URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.gz
        URL_HASH SHA256=954a01219bf818c7fb850fa610c2c8c71a4fa28fa32a1900056bcb6ff58cf908
        OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON"
        "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES unordered\\\;regex" # Note the escapes!
)

CPMAddPackage(
        NAME cereal
        GIT_REPOSITORY https://github.com/USCiLab/cereal
        GIT_TAG v1.3.2
        OPTIONS
        "BUILD_DOC OFF"
        "BUILD_SANDBOX OFF"
        "SKIP_PERFORMANCE_COMPARISON ON"
        SYSTEM ON
)

CPMAddPackage(
        NAME embed
        GIT_REPOSITORY https://github.com/batterycenter/embed
        GIT_TAG v1.2.19
        OPTIONS
        "B_PRODUCTION_MODE ON"
        SYSTEM ON
)

CPMAddPackage(
        NAME glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG f7485100cb16498f202f64d21b567c3788efa234
        SYSTEM ON
)
target_compile_definitions(glm PUBLIC GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE)

CPMAddPackage(
        NAME happly
        GIT_REPOSITORY https://github.com/nmwsharp/happly
        GIT_TAG master
        DOWNLOAD_ONLY ON
        SYSTEM ON
)
if (happly_ADDED)
    add_library(happly INTERFACE)
    target_include_directories(happly INTERFACE SYSTEM ${happly_SOURCE_DIR})
endif ()

CPMAddPackage(
        NAME icecream-cpp
        GIT_REPOSITORY https://github.com/renatoGarcia/icecream-cpp
        GIT_TAG v1.0.0
        SYSTEM ON
)

CPMAddPackage(
        NAME libien
        GIT_REPOSITORY "https://github.com/ien646/libien"
        GIT_TAG "master"
        OPTIONS "LIBIEN_BUILD_IMAGE ON"
)

CPMAddPackage(
        NAME lua
        GIT_REPOSITORY https://github.com/lua/lua
        GIT_TAG v5.4.7
        DOWNLOAD_ONLY YES
)
if (lua_ADDED)
    file(GLOB_RECURSE LUA_SOURCES ${lua_SOURCE_DIR}/*.c)
    list(FILTER LUA_SOURCES EXCLUDE REGEX "^.*/onelua\.c$")
    add_library(lua ${LUA_SOURCES})
    target_include_directories(lua PUBLIC SYSTEM ${lua_SOURCE_DIR})
endif ()

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        GIT_TAG v0.9.6
        SYSTEM ON
)

CPMAddPackage(
        NAME nativefiledialog-extended
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
        GIT_TAG v1.2.1
        OPTIONS "NFD_PORTAL ON"
        SYSTEM ON
)

CPMAddPackage(
        NAME sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.24
        SYSTEM ON
        OPTIONS "SDL_TEST OFF"
                "SDL_SHARED OFF"
                "SDL_STATIC ON"
)

CPMAddPackage(
        NAME sol2
        GIT_REPOSITORY https://github.com/ThePhD/sol2
        GIT_TAG v3.5.0
        SYSTEM ON
)

CPMAddPackage(
        NAME vk-bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG v1.3.288
        SYSTEM ON
)

CPMAddPackage(
        NAME VulkanMemoryAllocator
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        GIT_TAG v3.1.0
        SYSTEM ON
)

if (CATHEDRAL_BUILD_TESTS)
    CPMAddPackage(
            NAME catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2
            GIT_TAG v3.8.0
            SYSTEM ON
    )
endif ()
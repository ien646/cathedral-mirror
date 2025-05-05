set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cache)
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

if(WIN32 AND DEFINED ENV{QT_DIR})
    set(CMAKE_PREFIX_PATH $ENV{QT_DIR})
endif()
find_package(Qt6 COMPONENTS Concurrent Core OpenGL OpenGLWidgets Widgets REQUIRED)

find_package(OpenMP REQUIRED)

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
)
target_compile_definitions(glm PUBLIC GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE)

CPMAddPackage(
    NAME happly
    GIT_REPOSITORY https://github.com/nmwsharp/happly
    GIT_TAG master
    DOWNLOAD_ONLY ON
)
if(happly_ADDED)
    add_library(happly INTERFACE)
    target_include_directories(happly INTERFACE SYSTEM ${happly_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME icecream-cpp
    GIT_REPOSITORY https://github.com/renatoGarcia/icecream-cpp
    GIT_TAG v1.0.0
)

CPMAddPackage(
    NAME libien
    GIT_REPOSITORY "https://github.com/ien646/libien"
    GIT_TAG "master"
    OPTIONS "LIBIEN_BUILD_IMAGE ON"
)

CPMAddPackage(
    NAME magic_enum
    GITHUB_REPOSITORY Neargye/magic_enum
    GIT_TAG v0.9.6
)

CPMAddPackage(
    NAME vk-bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG v1.3.288
)

CPMAddPackage(
    NAME VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG v3.1.0
    SYSTEM ON
)

if(CATHEDRAL_BUILD_TESTS)
    CPMAddPackage(
        NAME catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.8.0
        SYSTEM ON
    )
endif()
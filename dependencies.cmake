include("cmake/CPM.cmake")

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

CPMAddPackage(
    NAME glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 4eb3fe1d7d8fd407cc7ccfa801a0311bb7dd281c
)

CPMAddPackage(
    NAME happly
    GIT_REPOSITORY https://github.com/nmwsharp/happly
    GIT_TAG master
    DOWNLOAD_ONLY ON
)
if(happly_ADDED)
    add_library(happly INTERFACE)
    target_include_directories(happly INTERFACE ${happly_SOURCE_DIR})
endif()

CPMAddPackage(
    NAME libien
    GIT_REPOSITORY "https://github.com/ien646/libien"
    GIT_TAG "master"
    OPTIONS "LIBIEN_BUILD_IMAGE ON"
)

CPMAddPackage(
    NAME nlohmann-json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
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
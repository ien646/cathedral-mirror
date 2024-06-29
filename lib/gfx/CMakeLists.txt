include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Vulkan.cmake)

set(TARGET_NAME cathedral-gfx)

add_library(${TARGET_NAME}
    "src/buffers/generic_buffer.cpp"
    "src/buffers/staging_buffer.cpp"
    "src/buffers/storage_buffer.cpp"
    "src/buffers/uniform_buffer.cpp"
    "src/buffers/vertex_buffer.cpp"

    "src/image.cpp"
    "src/shader.cpp"
    "src/swapchain.cpp"
    "src/vulkan_context.cpp"
    "src/vk_mem_alloc.cpp"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(${TARGET_NAME} PUBLIC
    VULKAN_HPP_NO_SETTERS
    VULKAN_HPP_NO_CONSTRUCTORS
    VULKAN_HPP_NO_TO_STRING
)

target_link_libraries(${TARGET_NAME} PUBLIC
    cathedral-core

    vk-bootstrap
    Vulkan::Vulkan
    VulkanMemoryAllocator
    libien
    Vulkan::shaderc_combined

    ${LIB_SPIRV_CROSS_CORE}
    ${LIB_SPIRV_CROSS_CPP}
    ${LIB_SPIRV_CROSS_GLSL}
    ${LIB_SPIRV_CROSS_REFLECT}
)

target_include_directories(${TARGET_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${VULKAN_INCLUDE_DIR}
)
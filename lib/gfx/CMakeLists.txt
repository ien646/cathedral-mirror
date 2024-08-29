include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Vulkan.cmake)

set(TARGET_NAME cathedral-gfx)

add_library(${TARGET_NAME}
    "src/buffers/generic_buffer.cpp"
    "src/buffers/index_buffer.cpp"
    "src/buffers/staging_buffer.cpp"
    "src/buffers/storage_buffer.cpp"
    "src/buffers/uniform_buffer.cpp"
    "src/buffers/vertex_buffer.cpp"

    "src/depthstencil_attachment.cpp"
    "src/descriptor_set_definition.cpp"
    "src/image.cpp"
    "src/pipeline.cpp"
    "src/sampler.cpp"
    "src/shader.cpp"
    "src/swapchain.cpp"
    "src/vulkan_context.cpp"
    "src/vk_mem_alloc.cpp"
)

target_compile_definitions(${TARGET_NAME} PUBLIC
    VULKAN_HPP_NO_SETTERS
    VULKAN_HPP_NO_CONSTRUCTORS
    VULKAN_HPP_NO_TO_STRING
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
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

target_precompile_headers(${TARGET_NAME} PUBLIC
    <vulkan/vulkan.h>
    <vulkan/vulkan.hpp>
    <vk_mem_alloc.h>
)
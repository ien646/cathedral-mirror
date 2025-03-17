set(SPIRV_REFLECT_SOURCE_FILE $ENV{VULKAN_SDK}/include/SPIRV-Reflect/spirv_reflect.c)
set(SPIRV_REFLECT_INCLUDE_DIR $ENV{VULKAN_SDK}/include/SPIRV-Reflect)

add_library(spirv-reflect ${SPIRV_REFLECT_SOURCE_FILE})
target_include_directories(spirv-reflect PUBLIC ${SPIRV_REFLECT_INCLUDE_DIR})

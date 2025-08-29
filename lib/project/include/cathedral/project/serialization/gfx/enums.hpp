#pragma once

#include <cathedral/gfx/shader_data_types.hpp>
#include <cathedral/project/asset_macros.hpp>

#include <vulkan/vulkan.hpp>

CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::gfx::shader_data_type);

CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerAddressMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerMipmapMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::Filter);
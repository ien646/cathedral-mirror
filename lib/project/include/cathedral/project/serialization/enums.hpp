#pragma once

#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/shader_bindings.hpp>
#include <cathedral/engine/texture_compression.hpp>

#include <cathedral/gfx/shader_data_types.hpp>

#include <vulkan/vulkan.hpp>

#include <magic_enum.hpp>

#define CATHEDRAL_SERIALIZE_ENUM_AUTO(in_class)                                                                             \
    namespace cereal                                                                                                        \
    {                                                                                                                       \
        template <typename Archive>                                                                                         \
        std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME([[maybe_unused]] const Archive& ar, const in_class& binding)          \
        {                                                                                                                   \
            return std::string{ magic_enum::enum_name(binding) };                                                           \
        }                                                                                                                   \
                                                                                                                            \
        template <typename Archive>                                                                                         \
        void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(                                                                             \
            [[maybe_unused]] const Archive& ar,                                                                             \
            in_class& binding,                                                                                              \
            const std::string& value)                                                                                       \
        {                                                                                                                   \
            const auto opt = magic_enum::enum_cast<in_class>(value);                                                        \
            CRITICAL_CHECK(opt.has_value(), "Invalid enum value");                                                          \
            binding = *opt;                                                                                                 \
        }                                                                                                                   \
    }

CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::gfx::shader_data_type);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::texture_compression_type);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_material_uniform_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_node_uniform_binding);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::material_domain);

CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerAddressMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerMipmapMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::Filter);
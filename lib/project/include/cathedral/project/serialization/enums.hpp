#pragma once

#include <cathedral/gfx/shader_data_types.hpp>
#include <cathedral/engine/texture_compression.hpp>
#include <cathedral/engine/shader_uniform_bindings.hpp>

#include <vulkan/vulkan_enums.hpp>

#include <cathedral/cereal_serializers.hpp>

#define CATHEDRAL_SERIALIZE_ENUM_AUTO(_class)                                                                                 \
    namespace cereal                                                                                                          \
    {                                                                                                                         \
        template <typename Archive>                                                                                           \
        std::string CEREAL_SAVE_MINIMAL_FUNCTION_NAME([[maybe_unused]] const Archive& ar, const _class& binding)              \
        {                                                                                                                     \
            return std::string{ magic_enum::enum_name(binding) };                                                             \
        }                                                                                                                     \
                                                                                                                              \
        template <typename Archive>                                                                                           \
        void CEREAL_LOAD_MINIMAL_FUNCTION_NAME([[maybe_unused]] const Archive& ar, _class& binding, const std::string& value) \
        {                                                                                                                     \
            const auto opt = magic_enum::enum_cast<_class>(value);                                                            \
            CRITICAL_CHECK(opt.has_value());                                                                                  \
            binding = *opt;                                                                                                   \
        }                                                                                                                     \
    }

CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::gfx::shader_data_type);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::texture_compression_type);
CATHEDRAL_SERIALIZE_ENUM_AUTO(cathedral::engine::shader_uniform_binding);

CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerAddressMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::SamplerMipmapMode);
CATHEDRAL_SERIALIZE_ENUM_AUTO(vk::Filter);
#pragma once

#include <cstdint>

namespace cathedral::engine
{
    constexpr uint32_t SCENE_DESCRIPTOR_SET_INDEX = 0;
    constexpr uint32_t MATERIAL_DESCRIPTOR_SET_INDEX = 1;
    constexpr uint32_t NODE_DESCRIPTOR_SET_INDEX = 2;

    constexpr uint32_t UNIFORM_BUFFER_BINDING_INDEX = 0;
    constexpr uint32_t SAMPLER_BINDING_INDEX = 1;
    constexpr uint32_t STORAGE_BUFFER_FIRST_BINDING_INDEX = 2;
} // namespace cathedral::engine
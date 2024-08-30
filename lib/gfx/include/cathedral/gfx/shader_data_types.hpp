#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <cstdint>

namespace cathedral::gfx
{
    // clang-format off
    enum class shader_data_type
    {
        BOOL, INT, UINT, FLOAT, DOUBLE,

        BVEC2, BVEC3, BVEC4,
        IVEC2, IVEC3, IVEC4,
        VEC2, VEC3, VEC4,
        UVEC2, UVEC3, UVEC4,
        DVEC2, DVEC3, DVEC4,

        MAT2X2, MAT2X3, MAT2X4, 
        MAT3X2, MAT3X3, MAT3X4, 
        MAT4X2, MAT4X3, MAT4X4,

        MAT2 = MAT2X2,
        MAT3 = MAT3X3,
        MAT4 = MAT4X4
    };
    // clang-format on

    constexpr uint32_t shader_data_type_size(shader_data_type type)
    {
        using enum shader_data_type;
        switch (type)
        {
        case BOOL:
        case INT:
        case UINT:
        case FLOAT:
            return 4;
        case DOUBLE:
        case BVEC2:
        case IVEC2:
        case VEC2:
        case UVEC2:
            return 8;
        case BVEC3:
        case IVEC3:
        case VEC3:
        case UVEC3:
            return 12;
        case BVEC4:
        case IVEC4:
        case VEC4:
        case UVEC4:
        case MAT2X2:
        case DVEC2:
            return 16;
        case MAT2X3:
        case MAT3X2:
        case DVEC3:
            return 24;
        case MAT2X4:
        case MAT4X2:
        case DVEC4:
            return 32;
        case MAT3X3:
            return 36;
        case MAT3X4:
        case MAT4X3:
            return 48;
        case MAT4X4:
            return 64;
        }
        CRITICAL_ERROR("Unhandled shader data type");
    }

    constexpr uint32_t shader_data_type_alignment(shader_data_type type)
    {
        using enum shader_data_type;
        switch (type)
        {
        case BOOL:
        case INT:
        case UINT:
        case FLOAT:
            return gfx::uniform_alignment<float>();
        case DOUBLE:
            return gfx::uniform_alignment<double>();
        case BVEC2:
        case VEC2:
        case IVEC2:
        case UVEC2:
            return gfx::uniform_alignment<glm::vec2>();
        case BVEC3:
        case VEC3:
        case IVEC3:
        case UVEC3:
            return gfx::uniform_alignment<glm::vec3>();
        case BVEC4:
        case VEC4:
        case IVEC4:
        case UVEC4:
        case DVEC2:
            return gfx::uniform_alignment<glm::vec4>();
        case MAT2X2:
        case MAT2X3:
        case MAT2X4:
        case MAT3X2:
        case MAT3X3:
        case MAT3X4:
        case MAT4X2:
        case MAT4X3:
        case MAT4X4:
        case DVEC3:
        case DVEC4:
            return gfx::uniform_alignment<glm::mat4>();
        }
        CRITICAL_ERROR("Unhandled shader data type");
    }

    constexpr uint32_t shader_data_type_offset(shader_data_type type)
    {
        return shader_data_type_size(type) + (shader_data_type_size(type) % shader_data_type_alignment(type));
    }
} // namespace cathedral::gfx
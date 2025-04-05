#pragma once

#include <cathedral/gfx/aligned_uniform.hpp>

#include <cstdint>

namespace cathedral::gfx
{
    // clang-format off
    enum class shader_data_type : uint8_t
    {
        BOOL, INT, UINT, FLOAT, DOUBLE,

        BVEC2, BVEC3, BVEC4,
        IVEC2, IVEC3, IVEC4,
        VEC2, VEC3, VEC4,
        UVEC2, UVEC3, UVEC4,
        DVEC2, DVEC3, DVEC4,

        MAT2X2, MAT2X3, MAT2X4, 
        MAT3X2, MAT3X3, MAT3X4, 
        MAT4X2, MAT4X3, MAT4X4
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

    constexpr uint32_t shader_data_type_offset(shader_data_type type, uint32_t count, uint32_t from_offset)
    {
        const uint32_t padding = from_offset % shader_data_type_alignment(type);
        return padding + (shader_data_type_size(type) * count);
    }

    constexpr const char* shader_data_type_glslstr(shader_data_type type)
    {
        using enum shader_data_type;
        switch (type)
        {
        case BOOL:
            return "bool";
        case INT:
            return "int";
        case UINT:
            return "uint";
        case FLOAT:
            return "float";
        case DOUBLE:
            return "double";
        case BVEC2:
            return "bvec2";
        case VEC2:
            return "vec2";
        case IVEC2:
            return "ivec2";
        case UVEC2:
            return "uvec2";
        case BVEC3:
            return "bvec3";
        case VEC3:
            return "vec3";
        case IVEC3:
            return "ivec3";
        case UVEC3:
            return "uvec3";
        case BVEC4:
            return "bvec4";
        case VEC4:
            return "vec4";
        case IVEC4:
            return "ivec4";
        case UVEC4:
            return "uvec4";
        case DVEC2:
            return "dvec2";
        case MAT2X2:
            return "mat2x2";
        case MAT2X3:
            return "mat2x3";
        case MAT2X4:
            return "mat2x4";
        case MAT3X2:
            return "mat3x2";
        case MAT3X3:
            return "mat3x3";
        case MAT3X4:
            return "mat3x4";
        case MAT4X2:
            return "mat4x2";
        case MAT4X3:
            return "mat4x3";
        case MAT4X4:
            return "mat4x4";
        case DVEC3:
            return "dvec3";
        case DVEC4:
            return "dvec4";
        }
        CRITICAL_ERROR("Unhandled shader data type");
    }

    constexpr const char* shader_data_type_cppstr(shader_data_type type)
    {
        using enum shader_data_type;
        switch (type)
        {
        case BOOL:
            return "bool";
        case INT:
            return "int32_t";
        case UINT:
            return "uint32_t";
        case FLOAT:
            return "float";
        case DOUBLE:
            return "double";
        case BVEC2:
            return "glm::bvec2";
        case VEC2:
            return "glm::vec2";
        case IVEC2:
            return "glm::ivec2";
        case UVEC2:
            return "glm::uvec2";
        case BVEC3:
            return "glm::bvec3";
        case VEC3:
            return "glm::vec3";
        case IVEC3:
            return "glm::ivec3";
        case UVEC3:
            return "glm::uvec3";
        case BVEC4:
            return "glm::bvec4";
        case VEC4:
            return "glm::vec4";
        case IVEC4:
            return "glm::ivec4";
        case UVEC4:
            return "glm::uvec4";
        case DVEC2:
            return "glm::dvec2";
        case MAT2X2:
            return "glm::mat2x2";
        case MAT2X3:
            return "glm::mat2x3";
        case MAT2X4:
            return "glm::mat2x4";
        case MAT3X2:
            return "glm::mat3x2";
        case MAT3X3:
            return "glm::mat3x3";
        case MAT3X4:
            return "glm::mat3x4";
        case MAT4X2:
            return "glm::mat4x2";
        case MAT4X3:
            return "glm::mat4x3";
        case MAT4X4:
            return "glm::mat4x4";
        case DVEC3:
            return "glm::dvec3";
        case DVEC4:
            return "glm::dvec4";
        }
        CRITICAL_ERROR("Unhandled shader data type");
    }

    std::optional<shader_data_type> shader_data_type_from_glslstr(const std::string& str);
} // namespace cathedral::gfx
#include <cathedral/gfx/shader_data_types.hpp>

namespace cathedral::gfx
{
    const std::unordered_map<std::string, shader_data_type> glsl_to_shader_data_type = {
        { "bool", shader_data_type::BOOL },     { "int32_t", shader_data_type::INT },
        { "uint32_t", shader_data_type::UINT }, { "float", shader_data_type::FLOAT },
        { "double", shader_data_type::DOUBLE }, { "bvec2", shader_data_type::BVEC2 },
        { "vec2", shader_data_type::VEC2 },     { "ivec2", shader_data_type::IVEC2 },
        { "uvec2", shader_data_type::UVEC2 },   { "bvec3", shader_data_type::BVEC3 },
        { "vec3", shader_data_type::VEC3 },     { "ivec3", shader_data_type::IVEC3 },
        { "uvec3", shader_data_type::UVEC3 },   { "bvec4", shader_data_type::BVEC4 },
        { "vec4", shader_data_type::VEC4 },     { "ivec4", shader_data_type::IVEC4 },
        { "uvec4", shader_data_type::UVEC4 },   { "mat2x2", shader_data_type::MAT2X2 },
        { "mat2x3", shader_data_type::MAT2X3 }, { "mat2x4", shader_data_type::MAT2X4 },
        { "mat3x2", shader_data_type::MAT3X2 }, { "mat3x3", shader_data_type::MAT3X3 },
        { "mat3x4", shader_data_type::MAT3X4 }, { "mat4x2", shader_data_type::MAT4X2 },
        { "mat4x3", shader_data_type::MAT4X3 }, { "mat4x4", shader_data_type::MAT4X4 },
        { "dvec2", shader_data_type::DVEC2 },   { "dvec3", shader_data_type::DVEC3 },
        { "dvec4", shader_data_type::DVEC4 },   { "mat2", shader_data_type::MAT2X2 },
        { "mat3", shader_data_type::MAT3X3 },   { "mat4", shader_data_type::MAT4X4 }
    };

    std::optional<shader_data_type> shader_data_type_from_glslstr(const std::string& str)
    {
        CRITICAL_CHECK(glsl_to_shader_data_type.contains(str), "Unhandled glsl data type");
        return glsl_to_shader_data_type.at(str);
    }
} // namespace cathedral::gfx
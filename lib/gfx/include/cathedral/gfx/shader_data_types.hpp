#pragma once

namespace cathedral::gfx
{
    enum class shader_data_type
    {
        BOOL,
        INT,
        UINT,
        FLOAT,
        DOUBLE,

        BVEC2,
        BVEC3,
        BVEC4,

        IVEC2,
        IVEC3,
        IVEC4,

        VEC2,
        VEC3,
        VEC4,

        UVEC2,
        UVEC3,
        UVEC4,

        DVEC2,
        DVEC3,
        DVEC4,

        MAT2X2,
        MAT2X3,
        MAT2X4,
        MAT3X2,
        MAT3X3,
        MAT3X4,
        MAT4X2,
        MAT4X3,
        MAT4X4,

        MAT2 = MAT2X2,
        MAT3 = MAT3X3,
        MAT4 = MAT4X4
    };
} // namespace cathedral::gfx
#pragma once

#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/shader_data_types.hpp>

namespace cathedral::gfx
{
    struct shader_reflection_inout_variable
    {
        uint32_t location;
        gfx::shader_data_type type;
        std::string name;
    };

    struct shader_reflection_descriptor_set
    {
        uint32_t set;
        uint32_t binding;
        uint32_t count;
        gfx::shader_data_type type;
    };

    struct shader_reflection_info
    {
        std::vector<shader_reflection_inout_variable> inputs;
        std::vector<shader_reflection_inout_variable> outputs;
        std::vector<shader_reflection_descriptor_set> descriptor_sets;
    };

    shader_reflection_info get_shader_reflection_info(const gfx::shader& shader);
}
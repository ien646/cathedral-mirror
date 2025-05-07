#pragma once

#include <cathedral/gfx/shader.hpp>
#include <cathedral/gfx/shader_data_types.hpp>

namespace cathedral::gfx
{
    struct shader_reflection_inout_variable
    {
        uint32_t location;
        shader_data_type type;
        std::string name;
    };

    struct shader_reflection_descriptor_set
    {
        uint32_t set;
        uint32_t binding;
        uint32_t count;
        descriptor_type desc_type;
        std::string name;
        uint32_t size;
    };

    struct shader_reflection_info
    {
        std::vector<shader_reflection_inout_variable> inputs;
        std::vector<shader_reflection_inout_variable> outputs;
        std::vector<shader_reflection_descriptor_set> descriptor_sets;
    };

    shader_reflection_info get_shader_reflection_info(const shader& shader);
}
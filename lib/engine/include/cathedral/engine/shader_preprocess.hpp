#pragma once

#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/gfx/shader.hpp>

#include <ien/algorithm.hpp>

#include <string>

namespace cathedral::engine
{
    struct shader_preprocess_buffer_variable
    {
        std::string name;
        std::string decl_block;
    };

    struct shader_preprocess_data
    {
        std::string clean_source;
        std::string main_function_block;
        std::vector<shader_variable> material_uniform_vars;
        std::vector<shader_variable> node_uniform_vars;
        std::vector<std::string> material_textures;
        std::vector<std::string> node_textures;
        std::vector<shader_preprocess_buffer_variable> material_buffers;
        std::vector<shader_preprocess_buffer_variable> node_buffers;

        shader_preprocess_data merge(const shader_preprocess_data& other) const
        {
            shader_preprocess_data result = *this;
            result.main_function_block = {};
            result.clean_source = {};
            std::ranges::copy(other.material_uniform_vars, std::back_inserter(result.material_uniform_vars));
            std::ranges::copy(other.node_uniform_vars, std::back_inserter(result.node_uniform_vars));
            std::ranges::copy(other.material_textures, std::back_inserter(result.material_textures));
            std::ranges::copy(other.node_textures, std::back_inserter(result.node_textures));
            std::ranges::copy(other.material_buffers, std::back_inserter(result.material_buffers));
            std::ranges::copy(other.node_buffers, std::back_inserter(result.node_buffers));
            return result;
        }
    };

    std::expected<shader_preprocess_data, std::string> get_shader_preprocess_data(std::string_view source);

    std::expected<std::string, std::string> preprocess_shader(
        gfx::shader_type type,
        const shader_preprocess_data& merged_pp_data,
        const std::string& clean_source,
        const std::string& main_block);
} // namespace cathedral::engine
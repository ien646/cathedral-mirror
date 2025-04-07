#pragma once

#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/gfx/shader.hpp>

#include <ien/algorithm.hpp>

#include <string>

namespace cathedral::engine
{
    struct shader_preprocess_data
    {
        std::string clean_source;
        std::vector<shader_variable> material_vars;
        std::vector<shader_variable> node_vars;
        std::vector<std::string> material_textures;
        std::vector<std::string> node_textures;

        shader_preprocess_data merge(const shader_preprocess_data& other)
        {
            shader_preprocess_data result = *this;
            std::ranges::copy(other.material_vars, std::back_inserter(result.material_vars));
            std::ranges::copy(other.node_vars, std::back_inserter(result.node_vars));
            std::ranges::copy(other.material_textures, std::back_inserter(result.material_textures));
            std::ranges::copy(other.node_textures, std::back_inserter(result.node_textures));
            return result;
        }
    };

    std::expected<shader_preprocess_data, std::string> get_shader_preprocess_data(std::string_view source);

    std::expected<std::string, std::string> preprocess_shader(gfx::shader_type type, const shader_preprocess_data& pp_data);
} // namespace cathedral::engine
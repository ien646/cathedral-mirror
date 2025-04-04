#pragma once

#include <cathedral/engine/shader_variable.hpp>

#include <cathedral/gfx/shader.hpp>

#include <string>

namespace cathedral::engine
{
    struct shader_preprocess_result
    {
        std::string source;
        std::vector<shader_variable> material_vars;
        std::vector<shader_variable> node_vars;
        std::vector<std::string> material_textures;
        std::vector<std::string> node_textures;
    };

    std::expected<shader_preprocess_result, std::string> preprocess_shader(gfx::shader_type type, std::string_view source);
} // namespace cathedral::engine
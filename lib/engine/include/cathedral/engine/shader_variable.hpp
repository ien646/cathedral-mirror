#pragma once

#include <cathedral/gfx/shader_data_types.hpp>
#include <cathedral/engine/shader_uniform_bindings.hpp>

namespace cathedral::engine
{
    struct shader_variable
    {
        shader_variable() = default;

        shader_variable(
            gfx::shader_data_type type,
            uint32_t count,
            std::string name,
            std::optional<shader_uniform_binding> binding = std::nullopt)
            : type(type)
            , count(count)
            , name(std::move(name))
            , binding(binding)
        {
        }

        gfx::shader_data_type type;
        uint32_t count = 0;
        std::string name = "undefined";
        std::optional<shader_uniform_binding> binding = std::nullopt;
    };
} // namespace cathedral::engine
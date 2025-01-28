#pragma once

#include <cathedral/engine/shader_uniform_bindings.hpp>
#include <cathedral/gfx/shader_data_types.hpp>
#include <cathedral/serializable.hpp>

namespace cathedral::engine
{
    struct shader_variable : serializable
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

        virtual ~shader_variable() = default;

        gfx::shader_data_type type;
        uint32_t count = 0;
        std::string name = "undefined";
        std::optional<shader_uniform_binding> binding = std::nullopt;

        nlohmann::json to_json() const override;
        void from_json(const nlohmann::json& json) override;
    };
} // namespace cathedral::engine
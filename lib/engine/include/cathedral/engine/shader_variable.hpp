#pragma once

#include <cathedral/engine/shader_bindings.hpp>
#include <cathedral/gfx/shader_data_types.hpp>

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

        shader_variable(const shader_variable&) = default;

        virtual ~shader_variable() = default;

        shader_variable& operator=(const shader_variable&) = default;

        gfx::shader_data_type type;
        uint32_t count = 0;
        std::string name = "undefined";
        std::optional<shader_uniform_binding> binding = std::nullopt;
    };

    namespace concepts
    {
        template <typename T>
        concept ShaderVariableType = ien::is_one_of_v<
            T,
            uint32_t,
            int32_t,
            float,
            double,

            glm::bvec2,
            glm::bvec3,
            glm::bvec4,
            glm::ivec2,
            glm::ivec3,
            glm::ivec4,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::uvec2,
            glm::uvec3,
            glm::uvec4,
            glm::dvec2,
            glm::dvec3,
            glm::dvec4,

            glm::mat2x2,
            glm::mat2x3,
            glm::mat2x4,
            glm::mat3x2,
            glm::mat3x3,
            glm::mat3x4,
            glm::mat4x2,
            glm::mat4x3,
            glm::mat4x4>;
    }

} // namespace cathedral::engine
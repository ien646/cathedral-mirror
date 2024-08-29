#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace cathedral::gfx
{
    template <typename T>
    constexpr int uniform_alignment()
    {
        if constexpr (std::is_class_v<T>)
        {
            return 16;
        }
        else if constexpr (std::is_scalar_v<T>)
        {
            return sizeof(T);
        }
        CRITICAL_ERROR("Unhandled type");
    }

    template <>
    constexpr int uniform_alignment<glm::vec2>()
    {
        return 8;
    }

    template <>
    constexpr int uniform_alignment<glm::uvec2>()
    {
        return 8;
    }

    template <>
    constexpr int uniform_alignment<glm::ivec2>()
    {
        return 8;
    }

    template <>
    constexpr int uniform_alignment<glm::vec3>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::uvec3>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::ivec3>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::vec4>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::uvec4>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::ivec4>()
    {
        return 16;
    }

    template <>
    constexpr int uniform_alignment<glm::mat4>()
    {
        return 16;
    }
}

#define CATHEDRAL_ALIGNED_UNIFORM(type, name) alignas(cathedral::gfx::uniform_alignment<type>()) type name
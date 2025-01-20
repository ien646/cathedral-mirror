#pragma once

#include <cathedral/core.hpp>

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <nlohmann/json.hpp>

namespace nlohmann
{
    // glm::xvec
    template <typename TElem, int Dim>
    struct adl_serializer<glm::vec<Dim, TElem>>
    {
        static void from_json(const json& j, glm::vec<Dim, TElem>& result)
        {
            CRITICAL_CHECK(j.is_array());
            int i = 0;
            for (const auto& v : j)
            {
                result[i++] = v;
            }
        }

        static glm::vec<Dim, TElem> from_json(const json& j)
        {
            glm::vec<Dim, TElem> result{static_cast<TElem>(0)};
            int i = 0;
            for (const auto& v : j)
            {
                result[i++] = v;
            }
            return result;
        }

        static void to_json(json& j, const glm::vec<Dim, TElem>& value)
        {
            auto result = json::array();
            for (int i = 0; i < value.length(); ++i)
            {
                result.push_back(value[i]);
            }
            j = result;
        }
    };

    // glm::mataxb
    template <typename TElem, int Cols, int Rows>
    struct adl_serializer<glm::mat<Cols, Rows, TElem>>
    {
        static void from_json(const json& j, glm::mat<Cols, Rows, TElem>& result)
        {
            CRITICAL_CHECK(j.is_array());
            int i = 0;
            for (const auto& v : j)
            {
                result[i++] = v;
            }
        }

        static glm::mat<Cols, Rows, TElem> from_json(const json& j)
        {
            glm::mat<Cols, Rows, TElem> result{static_cast<TElem>(1)};
            CRITICAL_CHECK(j.is_array());
            int i = 0;
            for (const auto& v : j)
            {
                result[i++] = v;
            }
            return result;
        }

        static void to_json(json& j, const glm::mat<Cols, Rows, TElem>& value)
        {
            auto result = json::array();
            for (int i = 0; i < value.length(); ++i)
            {
                result.push_back(value[i]);
            }
            j = result;
        }
    };
} // namespace nlohmann
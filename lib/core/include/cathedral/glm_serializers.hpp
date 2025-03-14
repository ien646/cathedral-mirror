#pragma once

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cereal/cereal.hpp>

namespace cereal
{
    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<2, TElem>& value)
    {
        archive(cereal::make_nvp("x", value.x), cereal::make_nvp("y", value.y));
    }

    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<3, TElem>& value)
    {
        archive(cereal::make_nvp("x", value.x), cereal::make_nvp("y", value.y), cereal::make_nvp("z", value.z));
    }

    template <typename Archive, typename TElem>
    void serialize(Archive& archive, glm::vec<4, TElem>& value)
    {
        archive(
            cereal::make_nvp("x", value.x),
            cereal::make_nvp("y", value.y),
            cereal::make_nvp("z", value.z),
            cereal::make_nvp("w", value.w));
    }

    template <typename Archive, typename TElem, glm::length_t X, glm::length_t Y>
    void serialize(Archive& ar, glm::mat<X, Y, TElem>& value)
    {
        for (size_t y = 0; y < Y; ++y)
        {
            for (size_t x = 0; x < X; ++x)
            {
                ar(cereal::make_nvp(std::format("[{}][{}]", x, y), value[x][y]));
            }
        }
    }
} // namespace cereal
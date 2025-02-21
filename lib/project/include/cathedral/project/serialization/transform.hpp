#pragma once

#include <cathedral/cereal_serializers.hpp>

#include <cathedral/engine/transform.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::transform& transform)
    {
        ar(cereal::make_nvp("position", transform.position()),
           cereal::make_nvp("rotation", transform.rotation()),
           cereal::make_nvp("scale", transform.scale()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::transform& transform)
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        ar(position, rotation, scale);

        transform.set_position(position);
        transform.set_rotation(rotation);
        transform.set_scale(scale);
    }
} // namespace cereal
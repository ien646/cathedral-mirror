#pragma once

#include <cathedral/cereal_serializers.hpp>

#include <cathedral/engine/camera.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::perspective_camera& camera)
    {
        ar(cereal::make_nvp("position", camera.position()),
           cereal::make_nvp("rotation", camera.rotation()),
           cereal::make_nvp("near_z", camera.near_z()),
           cereal::make_nvp("far_z", camera.far_z()),
           cereal::make_nvp("vfov", camera.vertical_fov()),
           cereal::make_nvp("aspect_ratio", camera.aspect_ratio()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::perspective_camera& camera)
    {
        glm::vec3 position;
        glm::vec3 rotation;
        float near_z;
        float far_z;
        float vfov;
        float aspect_ratio;

        ar(position, rotation, near_z, far_z, vfov, aspect_ratio);

        camera.set_position(position);
        camera.set_rotation(rotation);
        camera.set_near_z(near_z);
        camera.set_far_z(far_z);
        camera.set_vertical_fov(vfov);
        camera.set_aspect_ratio(aspect_ratio);
    }
} // namespace cereal
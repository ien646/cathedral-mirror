#pragma once

#include <cathedral/engine/camera.hpp>
#include <cathedral/engine/nodes/camera2d_node.hpp>

#include <cathedral/project/serialization/camera.hpp>
#include <cathedral/project/serialization/transform.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::camera2d_node& node)
    {
        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)),
           make_nvp("orthographic_camera", node.camera()),
           make_nvp("is_main_camera", node.is_main_camera()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::camera2d_node& node)
    {
        cathedral::engine::orthographic_camera camera;
        bool is_main_camera;

        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)), camera, is_main_camera);

        node.set_main_camera(is_main_camera);
        node.camera() = camera;
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::camera2d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::node, cathedral::engine::camera2d_node);
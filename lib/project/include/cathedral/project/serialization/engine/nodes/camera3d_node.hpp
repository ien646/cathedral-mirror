#pragma once

#include <cathedral/engine/nodes/camera3d_node.hpp>

#include <cathedral/project/serialization/engine/camera.hpp>
#include <cathedral/project/serialization/engine/transform.hpp>

#include <cathedral/glm_serializers.hpp>

#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <ranges>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::camera3d_node& node)
    {
        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)),
           make_nvp("perspective_camera", node.camera()),
           make_nvp("is_main_camera", node.is_main_camera()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::camera3d_node& node)
    {
        cathedral::engine::perspective_camera camera(0, 0, 0, 0);
        bool is_main_camera;

        ar(make_nvp("node", cereal::base_class<cathedral::engine::node>(&node)), camera, is_main_camera);

        node.set_main_camera(is_main_camera);
        node.camera() = camera;
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::camera3d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::node, cathedral::engine::camera3d_node);
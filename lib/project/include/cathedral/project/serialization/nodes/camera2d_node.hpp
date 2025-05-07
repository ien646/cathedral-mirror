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

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::camera2d_node& node)
    {
        ar(make_nvp("name", node.name()),
           make_nvp("type", std::string{ node.typestr() }),
           make_nvp("enabled", node.enabled()),
           make_nvp("children", node.children()),
           make_nvp("transform", node.get_local_transform()),
           make_nvp("orthographic_camera", node.camera()),
           make_nvp("is_main_camera", node.is_main_camera()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::camera2d_node& node)
    {
        std::string name;
        std::string type;
        bool enabled;
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> children;
        cathedral::engine::transform tform;
        cathedral::engine::orthographic_camera camera;
        bool is_main_camera;

        ar(name, type, enabled, children, tform, camera, is_main_camera);

        CRITICAL_CHECK(type == node.typestr(), "Invalid camera2d_node typestr");

        node.set_name(std::move(name));
        node.set_enabled(enabled);
        node.set_children(std::move(children));
        node.set_local_transform(tform);
        node.set_main_camera(is_main_camera);
        node.camera() = camera;
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::camera2d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::camera2d_node);
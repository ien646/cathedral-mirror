#pragma once

#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/cereal_serializers.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::mesh3d_node& node)
    {
        ar(cereal::make_nvp("name", node.name()),
           cereal::make_nvp("type", std::string{ node.typestr() }),
           cereal::make_nvp("enabled", node.enabled()),
           cereal::make_nvp("children", node.children()),
           cereal::make_nvp("transform", node.get_local_transform()),
           cereal::make_nvp("mesh_name", node.mesh_name()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::mesh3d_node& node)
    {
        std::string name;
        std::string type;
        bool enabled;
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> children;
        cathedral::engine::transform transform;
        std::string mesh_name;

        ar(name, type, enabled, children, transform, mesh_name);

        CRITICAL_CHECK(type == node.typestr());

        node.set_name(std::move(name));
        node.set_enabled(enabled);
        node.set_children(std::move(children));
        node.set_local_transform(transform);
        node.set_mesh(mesh_name);
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::mesh3d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::mesh3d_node);
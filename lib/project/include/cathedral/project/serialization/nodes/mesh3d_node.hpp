#pragma once

#include <cathedral/engine/nodes/mesh3d_node.hpp>

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
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::mesh3d_node& node)
    {
        std::optional material_name = !node.get_material().expired() ? node.get_material().lock()->name() : "";
        std::vector<std::string> bound_textures;
        for (const auto& tex : node.bound_textures())
        {
            bound_textures.push_back(tex->name());
        }

        // Filter out editor nodes
        auto child_nodes_range = node.children() |
                                 std::views::filter([](const std::shared_ptr<cathedral::engine::scene_node>& child) {
                                     return !child->name().starts_with("__");
                                 });
        const std::vector<std::shared_ptr<cathedral::engine::scene_node>> children = { child_nodes_range.begin(),
                                                                                       child_nodes_range.end() };

        ar(make_nvp("name", node.name()),
           make_nvp("type", std::string{ node.typestr() }),
           make_nvp("enabled", node.enabled()),
           make_nvp("children", children),
           make_nvp("transform", node.get_local_transform()),
           make_nvp("mesh_name", node.mesh_name()),
           make_nvp("material_name", material_name),
           make_nvp("node_textures", bound_textures));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::mesh3d_node& node)
    {
        std::string name;
        std::string type;
        bool enabled;
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> children;
        cathedral::engine::transform transform;
        std::optional<std::string> mesh_name;
        std::optional<std::string> material_name;
        std::vector<std::string> bound_textures;

        ar(name, type, enabled, children, transform, mesh_name, material_name, bound_textures);

        CRITICAL_CHECK(type == node.typestr(), "Invalid mesh3d_node typestr");

        node.set_name(std::move(name));
        node.set_enabled(enabled);
        node.set_children(std::move(children));
        node.set_local_transform(transform);
        if (mesh_name)
        {
            node.set_mesh(*mesh_name);
        }
        if (material_name)
        {
            node.set_material(material_name);
        }

        for (uint32_t i = 0; i < bound_textures.size(); ++i)
        {
            const auto& tex_name = bound_textures[i];
            if (tex_name == cathedral::engine::DEFAULT_TEXTURE_NAME)
            {
                continue;
            }
            node.bind_node_texture_slot(tex_name, i);
        }
    }
} // namespace cereal

CEREAL_REGISTER_TYPE(cathedral::engine::mesh3d_node);
CEREAL_REGISTER_POLYMORPHIC_RELATION(cathedral::engine::scene_node, cathedral::engine::mesh3d_node);
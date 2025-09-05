#pragma once

#include <cathedral/project/serialization/engine/nodes/camera2d_node.hpp>          // NOLINT
#include <cathedral/project/serialization/engine/nodes/camera3d_node.hpp>          // NOLINT
#include <cathedral/project/serialization/engine/nodes/directional_light_node.hpp> // NOLINT
#include <cathedral/project/serialization/engine/nodes/mesh3d_node.hpp>            // NOLINT
#include <cathedral/project/serialization/engine/nodes/node.hpp>                   // NOLINT
#include <cathedral/project/serialization/engine/nodes/point_light_node.hpp>       // NOLINT
#include <cathedral/project/serialization/engine/nodes/text_node.hpp>              // NOLINT

#include <cathedral/engine/scene.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::scene& scene)
    {
        // Filter out editor nodes
        auto root_nodes_range = scene.root_nodes() |
                                std::views::filter([](const std::unique_ptr<cathedral::engine::scene_node>& child) {
                                    return !child->name().starts_with("__");
                                }) |
                                std::views::transform([](const auto& v) { return v->copy(v->name(), true); });
        const std::vector<std::unique_ptr<cathedral::engine::scene_node>> root_nodes = { root_nodes_range.begin(),
                                                                                         root_nodes_range.end() };

        ar(make_nvp("root_nodes", root_nodes));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::scene& scene)
    {
        std::vector<std::unique_ptr<cathedral::engine::scene_node>> nodes;
        ar(nodes);

        // Regenerate child->parent references
        for (const auto& node : nodes)
        {
            node->fix_parent_references();
        }

        scene.load_nodes(std::move(nodes));
    }
} // namespace cereal
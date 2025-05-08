#pragma once

#include <cathedral/project/serialization/nodes/camera2d_node.hpp>
#include <cathedral/project/serialization/nodes/camera3d_node.hpp>
#include <cathedral/project/serialization/nodes/mesh3d_node.hpp>
#include <cathedral/project/serialization/nodes/node.hpp>
#include <cathedral/project/serialization/nodes/point_light_node.hpp>

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
                                std::views::filter([](const std::shared_ptr<cathedral::engine::scene_node>& child) {
                                    return !child->name().starts_with("__");
                                });
        const std::vector<std::shared_ptr<cathedral::engine::scene_node>> root_nodes = { root_nodes_range.begin(),
                                                                                         root_nodes_range.end() };

        ar(cereal::make_nvp("root_nodes", root_nodes));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::scene& scene)
    {
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> nodes;
        ar(nodes);
        scene.load_nodes(std::move(nodes));
    }
} // namespace cereal
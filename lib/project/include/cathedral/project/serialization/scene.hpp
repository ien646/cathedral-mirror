#pragma once

#include <cathedral/project/serialization/nodes/camera2d_node.hpp>
#include <cathedral/project/serialization/nodes/camera3d_node.hpp>
#include <cathedral/project/serialization/nodes/mesh3d_node.hpp>
#include <cathedral/project/serialization/nodes/node.hpp>

#include <cathedral/engine/scene.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>

namespace cereal
{
    template <typename Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const cathedral::engine::scene& scene)
    {
        ar(cereal::make_nvp("root_nodes", scene.root_nodes()));
    }

    template <typename Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, cathedral::engine::scene& scene)
    {
        std::vector<std::shared_ptr<cathedral::engine::scene_node>> nodes;
        ar(nodes);
        scene.load_nodes(std::move(nodes));
    }
} // namespace cereal
#pragma once

#include <cathedral/core.hpp>

#include <nlohmann/json_fwd.hpp>

FORWARD_CLASS(cathedral::engine, scene_node);
FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::engine
{
    std::shared_ptr<scene_node> construct_node_from_typestr(
        const std::string& typestr,
        scene& scn,
        scene_node* parent = nullptr);

    std::shared_ptr<scene_node> deserialize_child_from_json(
        const nlohmann::json& json,
        scene& scn,
        scene_node* parent = nullptr
    );
}
#include <cathedral/engine/serialization.hpp>

#include <cathedral/engine/nodes/node.hpp>

#include <nlohmann/json.hpp>

namespace cathedral::engine
{
    std::shared_ptr<scene_node> construct_node_from_typestr(
        const std::string& typestr,
        scene& scn,
        scene_node* parent)
    {
        if (typestr == NODE_TYPESTR)
        {
            return std::make_shared<node>(scn, "", parent);
        }
        CRITICAL_ERROR("Unhandled typestr in construction of scene_node-ish type");
    }

    std::shared_ptr<scene_node> deserialize_child_from_json(
        const nlohmann::json& json,
        scene& scn,
        scene_node* parent)
    {
        CRITICAL_CHECK(json.contains("type"));
        auto node = construct_node_from_typestr(json["type"].get<std::string>(), scn, parent);
        node->from_json(json);
        return node;
    }
} // namespace cathedral::engine
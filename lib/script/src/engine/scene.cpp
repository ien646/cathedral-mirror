#include <cathedral/script/engine/scene.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/scene.hpp>

constexpr auto ANNOTATIONS = R"lua(

---@class scene
---@field public add_root_node fun(name: string, type: node_type): scene_node
---@field public get_node fun(name: string): scene_node
---@field public remove_node fun(name: string)
---@field public root_nodes fun(): scene_node[]
---@field public get_nodes_by_type fun(type: node_type): scene_node[]
---@field public last_deltatime fun(): number
local scene = {}

)lua";

namespace cathedral::script::engine
{
    void scene_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, scene);
        // Re-enable as soon as renderer is exposed in scripts
        // AUTO_FUNC(get_renderer);
        AUTO_FUNC_OVERLOAD(
            add_root_node,
            std::shared_ptr<cathedral::engine::scene_node>,
            (const std::string&, cathedral::engine::node_type));
        // clang-format off
        AUTO_FUNC_OVERLOAD(get_node, std::shared_ptr<cathedral::engine::scene_node>, (const std::string&));
        // clang-format on
        AUTO_FUNC(remove_node);
        AUTO_FUNC(root_nodes);
        AUTO_FUNC(get_nodes_by_type);
        AUTO_FUNC(last_deltatime);
    }

    const std::string& scene_initializer::get_annotations()
    {
        static const std::string annotations = ANNOTATIONS;
        return annotations;
    }
} // namespace cathedral::script::engine
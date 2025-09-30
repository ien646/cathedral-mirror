#include <cathedral/script/engine/node.hpp>

#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/script/engine/scene_node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace
{
    const std::string annotations = R"lua(

---@class node : scene_node
---@field public local_position vec3
---@field public local_rotation vec3
---@field public local_scale vec3
---@field public world_position vec3
---@field public world_rotation vec3
---@field public world_scale vec3
---@field public local_transform transform
---@field public translate fun(self, offset: vec3)
---@field public rotate_degrees fun(self, degrees: vec3)
---@field public world_model_matrix fun(self): mat4
node = {{}}

---@type fun(snode: scene_node): node
function as_node(snode) end

)lua";
}

namespace cathedral::script::engine
{
    void node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, node);
        AUTO_BASE_CLASSES(cathedral::engine::scene_node);
        AUTO_PROPERTY("local_position", local_position, set_local_position);
        AUTO_PROPERTY("local_rotation", local_rotation, set_local_rotation);
        AUTO_PROPERTY("local_scale", local_scale, set_local_scale);
        AUTO_PROPERTY_READONLY("world_position", world_position);
        AUTO_PROPERTY_READONLY("world_rotation", world_rotation);
        AUTO_PROPERTY_READONLY("world_scale", world_scale);
        // clang-format off
        AUTO_PROPERTY_ADVANCED(
            "local_transform",
            CATHEDRAL_OVERLOAD_CONST(cathedral::engine::node, local_transform, const cathedral::engine::transform&, ()),
            &cathedral::engine::node::set_local_transform);
        // clang-format on
        AUTO_FUNC(translate);
        AUTO_FUNC(rotate_degrees);
        AUTO_FUNC(world_model_matrix);

        s.set_function("as_node", [](cathedral::engine::scene_node* node) {
            return dynamic_cast<cathedral::engine::node*>(node);
        });
    }

    const std::string& node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine
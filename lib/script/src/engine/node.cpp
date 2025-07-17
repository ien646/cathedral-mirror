#include "cathedral/script/engine/scene_node.hpp"
#include "ien/bits/str_utils/trim.hpp"

#include <cathedral/script/engine/node.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/nodes/node.hpp>

constexpr auto INHERITABLE_ANNOTATIONS = R"lua(

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

)lua";

constexpr auto ANNOTATIONS_FORMAT = R"lua(

---@class node
{0}
node = {{}}

)lua";

namespace cathedral::script::engine
{
    void node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, node);
        AUTO_BASE_CLASS(cathedral::engine::scene_node);
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
    }

    const std::string& node_initializer::get_annotations()
    {
        static const std::string annotations = std::format(ANNOTATIONS_FORMAT, get_inheritable_annotations());
        return annotations;
    }

    const std::string& node_initializer::get_inheritable_annotations()
    {
        static const std::string inheritable_annotations = scene_node_initializer{}.get_inheritable_annotations() + '\n' +
                                                           ien::str_trim(std::string{ INHERITABLE_ANNOTATIONS }, '\n');
        return inheritable_annotations;
    }
} // namespace cathedral::script::engine
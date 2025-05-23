#include <cathedral/script/engine/node.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/nodes/node.hpp>

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
        AUTO_PROPERTY("local_transform", get_local_transform, set_local_transform);
        AUTO_FUNC(get_world_model_matrix);
        AUTO_FUNC(type);
    }
} // namespace cathedral::script::engine
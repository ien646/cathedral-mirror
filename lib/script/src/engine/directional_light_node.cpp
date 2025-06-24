#include <cathedral/script/engine/directional_light_node.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/nodes/directional_light_node.hpp>

namespace cathedral::script
{
    void engine::directional_light_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, directional_light_node);
        AUTO_BASE_CLASS(cathedral::engine::node);
        AUTO_CTORS(AUTO_TYPE());
        AUTO_PROPERTY("color", color, set_color);
        AUTO_PROPERTY("intensity", intensity, set_intensity);
    }
} // namespace cathedral::script
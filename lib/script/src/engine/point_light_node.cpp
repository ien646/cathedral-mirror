#include <cathedral/script/engine/point_light_node.hpp>

#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace cathedral::script::engine
{
    void point_light_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, point_light_node);
        AUTO_BASE_CLASS(cathedral::engine::node);
        AUTO_CTORS(AUTO_TYPE());
        AUTO_PROPERTY("position", position, set_position);
        AUTO_PROPERTY("color", color, set_color);
        AUTO_PROPERTY("intensity", intensity, set_intensity);
        AUTO_PROPERTY("range", range, set_range);
        AUTO_PROPERTY("falloff_coefficient", falloff_coefficient, set_falloff_coefficient);
    }
} // namespace cathedral::script
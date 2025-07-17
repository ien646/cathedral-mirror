#include <cathedral/script/engine/point_light_node.hpp>

#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/script/engine/node.hpp>
#include <cathedral/script/init_macros.hpp>

constexpr auto ANNOTATIONS_FORMAT = R"lua(

---@class point_light_node
{0}
---@field public position vec3
---@field public color vec3
---@field public intensity number
---@field public range number
---@field public falloff_coefficient number
point_light_node = {{}}
)lua";

namespace cathedral::script::engine
{
    void point_light_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, point_light_node);
        AUTO_BASE_CLASS(cathedral::engine::node);
        AUTO_PROPERTY("position", position, set_position);
        AUTO_PROPERTY("color", color, set_color);
        AUTO_PROPERTY("intensity", intensity, set_intensity);
        AUTO_PROPERTY("range", range, set_range);
        AUTO_PROPERTY("falloff_coefficient", falloff_coefficient, set_falloff_coefficient);
    }

    const std::string& point_light_node_initializer::get_annotations()
    {
        static const std::string annotations =
            std::format(ANNOTATIONS_FORMAT, node_initializer{}.get_inheritable_annotations());
        return annotations;
    }
} // namespace cathedral::script::engine
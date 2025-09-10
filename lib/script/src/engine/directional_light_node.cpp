#include <cathedral/script/engine/directional_light_node.hpp>

#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/script/engine/node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace
{
    const std::string annotations = R"lua(
---@class directional_light_node : node
---@field public color vec3
---@field public intensity number
directional_light_node = {{}}
)lua";
}

namespace cathedral::script
{
    void engine::directional_light_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, directional_light_node);
        AUTO_BASE_CLASS(cathedral::engine::node);
        AUTO_PROPERTY("color", color, set_color);
        AUTO_PROPERTY("intensity", intensity, set_intensity);
    }

    const std::string& engine::directional_light_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script
#include <cathedral/script/engine/directional_light_node.hpp>

#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/script/engine/node.hpp>
#include <cathedral/script/init_macros.hpp>

namespace
{
    const std::string annotations = R"lua(
---@class directional_light_node: node
---@field public color vec3
---@field public intensity number
directional_light_node = {{}}

---@type fun(snode: scene_node): directional_light_node
function as_directional_light_node(snode) end
)lua";
}

namespace cathedral::script
{
    void engine::directional_light_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, directional_light_node);
        AUTO_BASE_CLASSES(cathedral::engine::node, cathedral::engine::scene_node);
        AUTO_PROPERTY("color", color, set_color);
        AUTO_PROPERTY("intensity", intensity, set_intensity);

        s.set_function("as_directional_light_node", [](cathedral::engine::scene_node* node) {
            return dynamic_cast<cathedral::engine::directional_light_node*>(node);
        });
    }

    const std::string& engine::directional_light_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script
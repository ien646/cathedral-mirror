#include <cathedral/script/engine/scene.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/scene.hpp>

namespace
{
    const std::string annotations = R"lua(

---@class scene
---@field public add_root_node fun(self, name: string, type: node_type): scene_node
---@field public get_node fun(self, name: string): scene_node
---@field public remove_node fun(self, name: string)
---@field public root_nodes fun(self): scene_node[]
---@field public get_nodes_by_type fun(self, type: node_type): scene_node[]
---@field public last_deltatime fun(self): number
---@field public keyboard_input fun(self): keyboard_input_interface
---@field public mouse_input fun(self): mouse_input_interface
scene = {}
)lua";
}

namespace cathedral::script::engine
{
    void scene_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, scene);
        // Re-enable as soon as renderer is exposed in scripts
        // AUTO_FUNC(get_renderer);
        AUTO_FUNC_OVERLOAD(add_root_node, cathedral::engine::scene_node*, (const std::string&, cathedral::engine::node_type));
        // clang-format off
        AUTO_FUNC_OVERLOAD(get_node, cathedral::engine::scene_node*, (const std::string&));
        // clang-format on
        AUTO_FUNC(remove_node);
        AUTO_STATE.set_function("root_nodes", [](const AUTO_TYPE& self) {
            std::vector<cathedral::engine::scene_node*> result;
            std::ranges::transform(self.root_nodes(), std::back_inserter(result), [](const auto& v) { return v.get(); });
            return result;
        });
        AUTO_FUNC(get_nodes_by_type);
        AUTO_FUNC(last_deltatime);
        AUTO_FUNC_NAMED(keyboard_input, get_keyboard_input_interface);
        AUTO_FUNC_NAMED(mouse_input, get_mouse_input_interface);
    }

    const std::string& scene_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine
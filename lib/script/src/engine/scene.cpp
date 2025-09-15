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
---@field public draw_debug_line fun(self, positions: vec4[], colors: vec4[], lifetime_seconds: number)
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
        AUTO_STATE.set_function(
            "draw_debug_line",
            [](AUTO_TYPE& self,
               std::vector<glm::vec4> positions,
               std::vector<glm::vec4> colors,
               const double lifetime_seconds) {
                if (positions.size() != colors.size())
                {
                    log_error("draw_debug_line called with mismatching number of positions and colors");
                    return;
                }
                std::vector<cathedral::engine::debug::line_vertex> vertices;
                vertices.reserve(positions.size());
                for (size_t i = 0; i < positions.size(); ++i)
                {
                    vertices.emplace_back(positions[i], colors[i]);
                }
                self.draw_debug_line(std::move(vertices), lifetime_seconds);
            });
    }

    const std::string& scene_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine
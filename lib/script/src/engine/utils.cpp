#include <cathedral/script/engine/utils.hpp>

#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/scene_node.hpp>

namespace
{
    const std::string annotations = R"(
---@type fun(n: scene_node): node
function as_node(n): node end

---@type fun(n: scene_node): node
function as_mesh3d_node(n): node end
)";
}

namespace cathedral::script::engine
{
    void utils_initializer::initialize(state& s)
    {
        s.set_function("as_node", [](cathedral::engine::scene_node* node) -> cathedral::engine::node* {
            return dynamic_cast<cathedral::engine::node*>(node);
        });

        s.set_function("as_mesh3d_node", [](cathedral::engine::scene_node* node) -> cathedral::engine::mesh3d_node* {
            return dynamic_cast<cathedral::engine::mesh3d_node*>(node);
        });
    }

    const std::string& utils_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine
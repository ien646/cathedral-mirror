#include <cathedral/script/engine/scene.hpp>

#include <cathedral/script/init_macros.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::script::engine
{
    void scene_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, scene);
        AUTO_FUNC(get_renderer);
        AUTO_FUNC_OVERLOAD(
            add_root_node,
            std::shared_ptr<cathedral::engine::scene_node>,
            (const std::string&, cathedral::engine::node_type));
        // clang-format off
        AUTO_FUNC_OVERLOAD(get_node, std::shared_ptr<cathedral::engine::scene_node>, (const std::string&));
        // clang-format on
        AUTO_FUNC(remove_node);
        AUTO_FUNC(root_nodes);
        AUTO_FUNC(get_nodes_by_type);
        AUTO_FUNC(last_deltatime);
    }
} // namespace cathedral::script::engine
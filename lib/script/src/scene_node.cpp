#include <cathedral/script/scene_node.hpp>

#include <cathedral/engine/scene_node.hpp>

#include <cathedral/script/init_macros.hpp>

namespace cathedral::script
{
    void scene_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, engine, scene_node);
        AUTO_FUNC(name);
        AUTO_FUNC(set_name);
        AUTO_FUNC(has_parent);
        AUTO_FUNC(parent);
        AUTO_FUNC(set_parent);
        AUTO_FUNC_OVERLOAD(add_child_node, std::shared_ptr<engine::scene_node>, (const std::string&, engine::node_type));
        AUTO_FUNC(children);
        AUTO_FUNC(set_children);
        AUTO_FUNC(get_child);
        AUTO_FUNC(remove_child);
        AUTO_FUNC(get_full_name);
        AUTO_FUNC(enable);
        AUTO_FUNC(disable);
        AUTO_FUNC(set_enabled);
        AUTO_FUNC(enabled);
        AUTO_FUNC(contains_child);
        AUTO_FUNC(is_editor_node);
        AUTO_FUNC(set_disabled_in_editor_mode);
        AUTO_FUNC(disabled_in_editor_mode);
    }
} // namespace cathedral::script
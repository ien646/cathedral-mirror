#include "cathedral/engine/nodes/node.hpp"

#include <cathedral/script/engine/scene_node.hpp>

#include <cathedral/engine/scene_node.hpp>

#include <cathedral/script/init_macros.hpp>

const std::string annotations = R"lua(

---@class scene_node
---@field public name fun(self): string
---@field public set_name fun(self, name: string)
---@field public has_parent fun(self): boolean
---@field public parent fun(self): scene_node
---@field public add_child_node fun(self, name: string, type: node_type): scene_node
---@field public children fun(self): scene_node[]
---@field public get_child fun(self, name: string): scene_node
---@field public remove_child fun(self, name: string)
---@field public get_full_name fun(self): string
---@field public enable fun(self)
---@field public disable fun(self)
---@field public set_enabled fun(self, enabled: boolean)
---@field public enabled fun(self): boolean
---@field public contains_child fun(self, name: string): boolean
---@field public is_editor_node fun(self): boolean
---@field public set_disabled_in_editor_mode fun(self, disabled: boolean)
---@field public disabled_in_editor_mode fun(self): boolean
---@field public type fun(self): node_type
---@field public typestr fun(self): string
scene_node = {{}}
)lua";

namespace cathedral::script::engine
{
    void scene_node_initializer::initialize(state& s)
    {
        AUTO_INIT_NEW_TYPE(s, cathedral::engine, scene_node);
        AUTO_FUNC(name);
        AUTO_FUNC(set_name);
        AUTO_FUNC(has_parent);
        AUTO_FUNC(parent);
        AUTO_FUNC_OVERLOAD(
            add_child_node,
            cathedral::engine::scene_node*,
            (const std::string&, cathedral::engine::node_type));
        AUTO_STATE.set_function("children", [](AUTO_TYPE* self) {
            std::vector<cathedral::engine::scene_node*> result;
            std::ranges::transform(self->children(), std::back_inserter(result), [](const auto& child) {
                return child.get();
            });
            return result;
        });
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
        AUTO_FUNC(type);
        AUTO_FUNC(typestr);
    }

    const std::string& scene_node_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script::engine
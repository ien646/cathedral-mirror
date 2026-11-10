#include <cathedral/engine/scene_object.hpp>

#include <cathedral/engine/node_factory.hpp>
#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/text_node.hpp>

#include <ien/algorithm.hpp>

#include <algorithm>
#include <atomic>

namespace cathedral::engine
{
    namespace
    {
        std::atomic_uint32_t uid_counter = 0;
    }

    scene_object::scene_object(std::string name, scene_object* parent, const bool enabled)
        : _uid(uid_counter++)
        , _name(MOVE(name))
        , _parent(parent)
        , _disabled(!enabled)
    {
    }

    std::string scene_object::get_full_name(const std::string& separator) const
    {
        std::string result = "";
        std::vector<const scene_object*> branch = get_node_branch();
        for (size_t i = 0; i < branch.size(); ++i)
        {
            result += branch[i]->name();
            if (i < branch.size() - 1)
            {
                result += separator;
            }
        }
        return result;
    }

    std::vector<scene_object*> scene_object::get_node_branch()
    {
        std::vector<scene_object*> result;
        result.push_back(this);

        auto* current_node = this;
        while (current_node->has_parent())
        {
            current_node = current_node->parent();
            result.push_back(current_node);
        }

        std::ranges::reverse(result);
        return result;
    }

    std::vector<const scene_object*> scene_object::get_node_branch() const
    {
        std::vector<const scene_object*> result;
        result.push_back(this);

        const auto* current_node = this;
        while (current_node->has_parent())
        {
            current_node = current_node->parent();
            result.push_back(current_node);
        }

        std::ranges::reverse(result);
        return result;
    }

    scene_object* scene_object::add_child_node(const std::string& name, const node_type type)
    {
        auto snode = construct_node(type);
        snode->set_name(name);
        return add_child_node(MOVE(snode));
    }

    void scene_object::remove_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](const auto& node) { return node->name() == name; });

        CRITICAL_CHECK(it != _children.end(), "Child node not found");
        ien::erase_unsorted(_children, it);
    }

    void scene_object::disable()
    {
        _disabled = true;
    }

    void scene_object::enable()
    {
        _disabled = false;
    }

    void scene_object::set_enabled(const bool enabled)
    {
        _disabled = !enabled;
    }

    bool scene_object::contains_child(const std::string& name) const
    {
        return std::ranges::find_if(_children, [&name](const auto& child) { return child->name() == name; })
               != _children.end();
    }

    scene_object* scene_object::get_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](auto& child) { return child->name() == name; });
        CRITICAL_CHECK(it != _children.end(), "Node not found");
        return it->get();
    }

    std::unique_ptr<scene_object> scene_object::orphan_child(const scene_object* node)
    {
        const auto it = std::ranges::find_if(_children, [node](const std::unique_ptr<scene_object>& child) {
            return child.get() == node;
        });
        if (it == _children.end())
        {
            log_error(std::format("Attempt to orphan non existent child '{}'", static_cast<const void*>(node)));
        }
        std::unique_ptr<scene_object> result(it->release());
        result->_parent = nullptr;
        _children.erase(it);
        return result;
    }

    std::unique_ptr<scene_object> scene_object::orphan_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](auto& child) { return child->name() == name; });
        if (it == _children.end())
        {
            log_error(std::format("Attempt to orphan non existent child '{}'", name));
            return {};
        }
        std::unique_ptr<scene_object> result(it->release());
        result->_parent = nullptr;
        _children.erase(it);
        return result;
    }

    bool scene_object::is_editor_node() const
    {
        return _name.starts_with("__");
    }

    void scene_object::set_disabled_in_editor_mode(const bool disabled)
    {
        _disabled_in_editor = disabled;
    }

    bool scene_object::disabled_in_editor_mode() const
    {
        return _disabled_in_editor;
    }

    void scene_object::add_script(const std::shared_ptr<script>& script)
    {
        _script_names.push_back(script->name());
        _scripts.push_back(script);
    }

    void scene_object::add_script(std::string name)
    {
        _script_names.push_back(MOVE(name));
        _scripts.push_back(nullptr);
    }

    void scene_object::remove_script(const size_t index)
    {
        CRITICAL_CHECK(_scripts.size() > index, "Script index out of range");
        _scripts.erase(_scripts.begin() + static_cast<uint32_t>(index));
        _script_names.erase(_script_names.begin() + static_cast<uint32_t>(index));
    }

    void scene_object::remove_script(const std::string& name)
    {
        const auto it = std::ranges::find(_script_names, name);
        if (it != _script_names.end())
        {
            const auto index = std::distance(_script_names.begin(), it);
            _script_names.erase(it);
            _scripts.erase(_scripts.begin() + index);
        }
    }

    const std::vector<std::shared_ptr<script>>& scene_object::scripts() const
    {
        return _scripts;
    }

    const std::vector<std::string>& scene_object::script_names() const
    {
        return _script_names;
    }

    void scene_object::fix_parent_references(scene_object* parent)
    {
        _parent = parent;
        for (const auto& node : _children)
        {
            node->fix_parent_references(this);
        }
    }

    scene_object* scene_object::add_child_node(std::unique_ptr<scene_object> node)
    {
        node->_parent = this;
        _children.push_back(MOVE(node));
        return _children.back().get();
    }
} // namespace cathedral::engine
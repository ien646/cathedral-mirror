#include <cathedral/engine/scene_node.hpp>

#include <cathedral/engine/node_type.hpp>
#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
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

    scene_node::scene_node(std::string name, scene_node* parent, const bool enabled)
        : _uid(uid_counter++)
        , _name(MOVE(name))
        , _parent(parent)
        , _disabled(!enabled)
    {
    }

    std::string scene_node::get_full_name(const std::string& separator) const
    {
        size_t resulting_size = _name.size();

        const auto* current_node = this;
        while (current_node->has_parent())
        {
            current_node = current_node->parent();
            resulting_size += current_node->name().size();
        }

        std::string result;
        result.reserve(resulting_size);

        result += name();
        current_node = this;

        while (current_node->has_parent())
        {
            current_node = current_node->parent();
            result += separator + name();
        }

        return result;
    }

    std::vector<scene_node*> scene_node::get_node_branch()
    {
        std::vector<scene_node*> result;
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

    std::vector<const scene_node*> scene_node::get_node_branch() const
    {
        std::vector<const scene_node*> result;
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

    scene_node* scene_node::add_child_node(const std::string& name, const node_type type)
    {
        switch (type)
        {
        case node_type::NODE:
            return add_child_node<node>(name);
        case node_type::MESH3D_NODE:
            return add_child_node<mesh3d_node>(name);
        case node_type::CAMERA2D_NODE:
            return add_child_node<camera2d_node>(name);
        case node_type::CAMERA3D_NODE:
            return add_child_node<camera3d_node>(name);
        case node_type::POINT_LIGHT:
            return add_child_node<point_light_node>(name);
        case node_type::DIRECTIONAL_LIGHT:
            return add_child_node<directional_light_node>(name);
        case node_type::TEXT_NODE:
            return add_child_node<text_node>(name);
        default:
            CRITICAL_ERROR("Unhandled node type");
        }
    }

    void scene_node::remove_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](const auto& node) { return node->name() == name; });

        CRITICAL_CHECK(it != _children.end(), "Child node not found");
        ien::erase_unsorted(_children, it);
    }

    void scene_node::disable()
    {
        _disabled = true;
    }

    void scene_node::enable()
    {
        _disabled = false;
    }

    void scene_node::set_enabled(const bool enabled)
    {
        _disabled = !enabled;
    }

    bool scene_node::contains_child(const std::string& name) const
    {
        return std::ranges::find_if(_children, [&name](const auto& child) { return child->name() == name; }) !=
               _children.end();
    }

    scene_node* scene_node::get_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](auto& child) { return child->name() == name; });
        CRITICAL_CHECK(it != _children.end(), "Node not found");
        return it->get();
    }

    std::unique_ptr<scene_node> scene_node::orphan_child(const scene_node* node)
    {
        const auto it = std::ranges::find_if(_children, [node](const std::unique_ptr<scene_node>& child) {
            return child.get() == node;
        });
        if (it == _children.end())
        {
            log_error(std::format("Attempt to orphan non existent child '{}'", static_cast<const void*>(node)));
        }
        std::unique_ptr<scene_node> result(it->release());
        result->_parent = nullptr;
        _children.erase(it);
        return result;
    }

    std::unique_ptr<scene_node> scene_node::orphan_child(const std::string& name)
    {
        const auto it = std::ranges::find_if(_children, [&name](auto& child) { return child->name() == name; });
        if (it == _children.end())
        {
            log_error(std::format("Attempt to orphan non existent child '{}'", name));
            return {};
        }
        std::unique_ptr<scene_node> result(it->release());
        result->_parent = nullptr;
        _children.erase(it);
        return result;
    }

    bool scene_node::is_editor_node() const
    {
        return _name.starts_with("__");
    }

    void scene_node::set_disabled_in_editor_mode(const bool disabled)
    {
        _disabled_in_editor = disabled;
    }

    bool scene_node::disabled_in_editor_mode() const
    {
        return _disabled_in_editor;
    }

    void scene_node::add_script(const std::shared_ptr<script>& script)
    {
        _script_names.push_back(script->name());
        _scripts.push_back(script);
    }

    void scene_node::add_script(std::string name)
    {
        _script_names.push_back(MOVE(name));
        _scripts.push_back(nullptr);
    }

    void scene_node::remove_script(const size_t index)
    {
        CRITICAL_CHECK(_scripts.size() > index, "Script index out of range");
        _scripts.erase(_scripts.begin() + static_cast<uint32_t>(index));
        _script_names.erase(_script_names.begin() + static_cast<uint32_t>(index));
    }

    void scene_node::remove_script(const std::string& name)
    {
        const auto it = std::ranges::find(_script_names, name);
        if (it != _script_names.end())
        {
            const auto index = std::distance(_script_names.begin(), it);
            _script_names.erase(it);
            _scripts.erase(_scripts.begin() + index);
        }
    }

    const std::vector<std::shared_ptr<script>>& scene_node::scripts() const
    {
        return _scripts;
    }

    const std::vector<std::string>& scene_node::script_names() const
    {
        return _script_names;
    }

    void scene_node::fix_parent_references(scene_node* parent)
    {
        _parent = parent;
        for (const auto& node : _children)
        {
            node->fix_parent_references(this);
        }
    }

    void scene_node::add_child_node(std::unique_ptr<scene_node> node)
    {
        node->_parent = this;
        _children.push_back(MOVE(node));
    }
} // namespace cathedral::engine
#include "cathedral/engine/node_type.hpp"
#include "cathedral/engine/nodes/camera2d_node.hpp"
#include <cathedral/engine/scene_node.hpp>

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <ien/algorithm.hpp>

#include <algorithm>

namespace cathedral::engine
{
    namespace
    {
        std::atomic_uint32_t uid_counter = 0;
    }

    scene_node::scene_node(std::string name, scene_node* parent, bool enabled)
        : _uid(uid_counter++)
        , _name(std::move(name))
        , _parent(parent)
        , _disabled(!enabled)
    {
    }

    std::string scene_node::get_full_name(const std::string& separator) const
    {
        size_t resulting_size = _name.size();

        const scene_node* current_node = this;
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

        scene_node* current_node = this;
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

        const scene_node* current_node = this;
        while (current_node->has_parent())
        {
            current_node = current_node->parent();
            result.push_back(current_node);
        }

        std::ranges::reverse(result);
        return result;
    }

    std::shared_ptr<engine::scene_node> scene_node::add_child_node(const std::string& name, node_type type)
    {
        switch (type)
        {
        case node_type::NODE:
            return add_child_node<engine::node>(name);
        case node_type::MESH3D_NODE:
            return add_child_node<engine::mesh3d_node>(name);
        case node_type::CAMERA2D_NODE:
            return add_child_node<engine::camera2d_node>(name);
        case node_type::CAMERA3D_NODE:
            return add_child_node<engine::camera3d_node>(name);
        default:
            CRITICAL_ERROR("Unhandled node type");
        }
    }

    std::shared_ptr<scene_node> scene_node::get_child(const std::string& name)
    {
        for (const auto& child : _children)
        {
            if (child->name() == name)
            {
                return child;
            }
        }
        return nullptr;
    }

    void scene_node::remove_child(const std::string& name)
    {
        auto it = std::ranges::find_if(_children, [&name](const auto& node) { return node->name() == name; });

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

    void scene_node::set_enabled(bool enabled)
    {
        _disabled = !enabled;
    }

    bool scene_node::contains_child(const std::string& name) const
    {
        return std::ranges::find_if(_children, [&name](const std::shared_ptr<scene_node>& child) {
                   return child->name() == name;
               }) != _children.end();
    }

    std::shared_ptr<engine::scene_node> scene_node::get_child(const std::string& name) const
    {
        auto it = std::ranges::find_if(_children, [&name](const std::shared_ptr<scene_node>& child) {
            return child->name() == name;
        });
        CRITICAL_CHECK(it != _children.end(), "Node not found");
        return *it;
    }

    void scene_node::add_child_node(std::shared_ptr<scene_node> node)
    {
        _children.push_back(std::move(node));
    }
} // namespace cathedral::engine
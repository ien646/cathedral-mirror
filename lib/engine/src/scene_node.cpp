#include <cathedral/engine/scene_node.hpp>

namespace cathedral::engine
{
    scene_node::scene_node(scene& scn, const std::string& name, scene_node* parent)
        : _scene(scn)
        , _name(name)
        , _parent(parent)
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
            result += name();
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

        std::reverse(result.begin(), result.end());
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

        std::reverse(result.begin(), result.end());
        return result;
    }

    std::shared_ptr<scene_node> scene_node::get_child(const std::string& name)
    {
        for (auto& child : _children)
        {
            if (child->name() == name)
            {
                return child;
            }
        }
        return nullptr;
    }

    void scene_node::disable()
    {
        _disabled = true;
    }

    void scene_node::enable()
    {
        _disabled = false;
    }
} // namespace cathedral::engine
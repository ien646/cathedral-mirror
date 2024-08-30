#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    glm::vec3 node::local_position() const
    {
        return _local_transform.position();
    }

    void node::set_local_position(glm::vec3 position)
    {
        _local_transform.set_position(position);
        _world_transform_needs_refresh = true;
    }

    glm::vec3 node::local_rotation() const
    {
        return _local_transform.rotation();
    }

    void node::set_local_rotation(glm::vec3 rotation)
    {
        _local_transform.set_rotation(rotation);
        _world_transform_needs_refresh = true;
    }

    glm::vec3 node::local_scale() const
    {
        return _local_transform.scale();
    }

    void node::set_local_scale(glm::vec3 scale)
    {
        _local_transform.set_scale(scale);
        _world_transform_needs_refresh = true;
    }

    const transform& node::get_local_transform() const
    {
        return _local_transform;
    }

    const transform& node::get_world_transform() const
    {
        if (_world_transform_needs_refresh)
        {
            recalculate_world_transform();
        }
        return _world_transform;
    }

    void node::tick(double deltatime)
    {
        if (_disabled)
        {
            return;
        }

        for (auto& child : _children)
        {
            child->tick(deltatime);
        }
    }

    void node::recalculate_world_transform() const
    {
        if (this->has_parent())
        {
            glm::vec3 current_scale = _local_transform.scale();
            glm::vec3 current_rotation = _local_transform.rotation();
            glm::vec3 current_position = _local_transform.position();

            const scene_node* current_node = this;
            while (current_node->has_parent())
            {
                current_node = current_node->parent();
                if (const node* enode = dynamic_cast<const node*>(current_node))
                {
                    current_scale += enode->local_scale();
                    current_rotation += enode->local_rotation();
                    current_position += enode->local_position();
                }
            }

            _world_transform.set_position(current_position);
            _world_transform.set_scale(current_scale);
            _world_transform.set_rotation(current_rotation);
        }
        else
        {
            _world_transform = _local_transform;
        }
    }
} // namespace cathedral::engine
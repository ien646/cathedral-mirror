#include <cathedral/engine/nodes/node.hpp>

#include <cathedral/json_serializers.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <nlohmann/json.hpp>

namespace cathedral::engine
{
    glm::vec3 node::local_position() const
    {
        return _local_transform.position();
    }

    void node::set_local_position(glm::vec3 position)
    {
        _local_transform.set_position(position);
        _world_model_needs_refresh = true;
    }

    glm::vec3 node::local_rotation() const
    {
        return _local_transform.rotation();
    }

    void node::set_local_rotation(glm::vec3 rotation)
    {
        _local_transform.set_rotation(rotation);
        _world_model_needs_refresh = true;
    }

    glm::vec3 node::local_scale() const
    {
        return _local_transform.scale();
    }

    void node::set_local_scale(glm::vec3 scale)
    {
        _local_transform.set_scale(scale);
        _world_model_needs_refresh = true;
    }

    const transform& node::get_local_transform() const
    {
        return _local_transform;
    }

    const glm::mat4& node::get_world_model_matrix() const
    {
        if(_world_model_needs_refresh)
        {
            recalculate_world_model();
        }
        return _world_model;
    }

    void node::tick(scene& scene, double deltatime)
    {
        if (_disabled)
        {
            return;
        }

        for (auto& child : _children)
        {
            child->tick(scene, deltatime);
        }
    }

    void node::editor_tick(scene& scene, double deltatime)
    {
        if (_disabled)
        {
            return;
        }

        for (auto& child : _children)
        {
            child->editor_tick(scene, deltatime);
        }
    }

    void node::recalculate_world_model() const
    {
        _world_model = _local_transform.get_model_matrix();

        const scene_node* current_node = this->parent();

        while (current_node != nullptr)
        {
            if (const auto* node3d = dynamic_cast<const node*>(current_node))
            {
                _world_model = node3d->get_local_transform().get_model_matrix() * _world_model;
            }
            current_node = current_node->parent();
        }
    }
} // namespace cathedral::engine
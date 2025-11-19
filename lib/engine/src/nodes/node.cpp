#include <cathedral/engine/nodes/node.hpp>

#include <cathedral/memory.hpp>

#include <cathedral/engine/scene.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace cathedral::engine
{
    glm::vec3 node::local_position() const
    {
        return _local_transform.position();
    }

    void node::set_local_position(const glm::vec3 position)
    {
        _local_transform.set_position(position);
        _world_model_needs_regen = true;
    }

    glm::vec3 node::local_rotation() const
    {
        return _local_transform.rotation();
    }

    void node::set_local_rotation(const glm::vec3 rotation)
    {
        _local_transform.set_rotation(rotation);
        _world_model_needs_regen = true;
    }

    glm::vec3 node::local_scale() const
    {
        return _local_transform.scale();
    }

    void node::set_local_scale(const glm::vec3 scale)
    {
        _local_transform.set_scale(scale);
        _world_model_needs_regen = true;
    }

    glm::vec3 node::world_position() const
    {
        const auto& w = world_model_matrix();
        return { w[3][0], w[3][1], w[3][2] };
    }

    glm::vec3 node::world_scale() const
    {
        const auto& w = world_model_matrix();
        return { glm::length(glm::xyz(w[0])), glm::length(glm::xyz(w[1])), glm::length(glm::xyz(w[2])) };
    }

    glm::vec3 node::world_rotation() const
    {
        const auto& w = world_model_matrix();
        auto rotation_matrix = glm::identity<glm::mat4>();
        rotation_matrix[0] = glm::normalize(w[0]);
        rotation_matrix[1] = glm::normalize(w[1]);
        rotation_matrix[2] = glm::normalize(w[2]);

        float x;
        float y;
        float z;
        glm::extractEulerAngleXYZ(rotation_matrix, x, y, z);
        return glm::degrees(glm::vec3{ x, y, z });
    }

    void node::set_local_transform(const transform& tform)
    {
        if (_local_transform != tform)
        {
            _local_transform = tform;
            _world_model_needs_regen = true;
        }
    }

    const transform& node::local_transform() const
    {
        return _local_transform;
    }

    void node::translate(const glm::vec3 translation)
    {
        _local_transform.translate(translation);
        _world_model_needs_regen = true;
    }

    void node::rotate_degrees(const glm::vec3 degrees)
    {
        _local_transform.rotate_degrees(degrees);
        _world_model_needs_regen = true;
    }

    const glm::mat4& node::world_model_matrix() const
    {
        if (_world_model_needs_regen)
        {
            recalculate_world_model();
        }
        else
        {
            // Check if any parent nodes need to recalculate world model matrix and
            // regenerate their cached world model matrix in order of left to right
            auto regen_nodes = get_scratch_vector<node*>();
            scene_node* parent_node = _parent;
            while (parent_node != nullptr)
            {
                if (auto* const node = dynamic_cast<engine::node*>(parent_node))
                {
                    if (node->_world_model_needs_regen)
                    {
                        regen_nodes.push_back(node);
                    }
                }
                parent_node = parent_node->parent();
            }

            std::ranges::reverse(regen_nodes);
            for (const auto* node : regen_nodes)
            {
                node->recalculate_world_model();
            }
        }

        return _world_model;
    }

    void node::tick_setup(scene& scene)
    {
        for (const auto& child : _children)
        {
            child->tick_setup(scene);
        }
    }

    void node::tick(scene& scene, const double deltatime)
    {
        if (_disabled)
        {
            return;
        }

        for (const auto& child : _children)
        {
            child->tick(scene, deltatime);
        }

        for (size_t i = 0; i < _scripts.size(); ++i)
        {
            auto& script = _scripts[i];
            if (script == nullptr)
            {
                script = scene.load_script(_script_names[i]);
            }
            script->tick(this, scene, deltatime);
        }

        scene.increase_node_count();
    }

    void node::editor_tick(scene& scene, const double deltatime)
    {
        if (_disabled || _disabled_in_editor)
        {
            return;
        }

        for (const auto& child : _children)
        {
            child->editor_tick(scene, deltatime);
        }

        for (size_t i = 0; i < _scripts.size(); ++i)
        {
            auto& script = _scripts[i];
            if (script == nullptr)
            {
                script = scene.load_script(_script_names[i]);
            }
            script->editor_tick(this, scene, deltatime);
        }
    }

    std::unique_ptr<scene_node> node::copy(const std::string& copy_name, const bool copy_children) const
    {
        auto result = std::make_unique<node>(copy_name, _parent, !_disabled);
        copy_into(*result, copy_children);
        return result;
    }

    void node::recalculate_world_model() const
    {
        const auto previous = _world_model;

        const auto local_model = _local_transform.get_model_matrix();

        if (_parent != nullptr)
        {
            if (const auto* parent_node = dynamic_cast<const node*>(_parent))
            {
                _world_model = parent_node->world_model_matrix() * local_model;
            }
        }
        else
        {
            _world_model = local_model;
        }

        if (previous != _world_model)
        {
            for (const auto& child : _children)
            {
                if (const auto child_node = dynamic_cast<node*>(child.get()))
                {
                    child_node->recalculate_world_model();
                }
            }
        }

        _world_model_needs_regen = false;
    }

    void node::copy_children_into(scene_node& target) const
    {
        for (const auto& child : _children)
        {
            auto copy = child->copy(child->name(), true);
            target.add_child_node(MOVE(copy));
        }
    }

    void node::copy_into(node& target, const bool copy_children) const
    {
        target.set_local_transform(_local_transform);
        for (const auto& name : _script_names)
        {
            target.add_script(name);
        }

        if (copy_children)
        {
            copy_children_into(target);
        }
    }

    template <>
    std::unique_ptr<node> construct_node<node>(std::string name, scene_node* parent, bool enabled)
    {
        return std::make_unique<node>(MOVE(name), parent, enabled);
    }
} // namespace cathedral::engine
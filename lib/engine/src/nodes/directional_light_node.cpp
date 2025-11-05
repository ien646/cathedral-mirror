#include <cathedral/engine/scene.hpp>

#include <cathedral/engine/nodes/directional_light_node.hpp>

#include <glm/ext/matrix_transform.hpp>

namespace cathedral::engine
{
    glm::vec3 directional_light_node::direction() const
    {
        return _data.direction;
    }

    glm::vec3 directional_light_node::color() const
    {
        return _data.color;
    }

    float directional_light_node::intensity() const
    {
        return _data.intensity;
    }

    void directional_light_node::set_color(const glm::vec3& color)
    {
        _data.color = color;
    }

    void directional_light_node::set_intensity(const float intensity)
    {
        _data.intensity = intensity;
    }

    void directional_light_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);
        if (_disabled)
        {
            return;
        }
        update_data(scene);
    }

    void directional_light_node::editor_tick(scene& scene, const double deltatime)
    {
        node::editor_tick(scene, deltatime);
        if (_disabled)
        {
            return;
        }
        update_data(scene);
    }

    std::unique_ptr<scene_node> directional_light_node::copy(const std::string& copy_name, const bool copy_children) const
    {
        auto result = std::make_unique<directional_light_node>(copy_name, _parent, !_disabled);

        node::copy_into(*result, copy_children);
        result->_data = _data;

        return result;
    }

    void directional_light_node::update_data(scene& scene)
    {
        constexpr glm::vec3 DIRECTION_ZERO = { 0.0F, 1.0F, 0.0F }; // Looking straight down

        glm::mat4 rotation = glm::rotate(glm::mat4{ 1.0F }, glm::radians(_local_transform.rotation().x), glm::vec3{ 1.0F, 0.0F, 0.0F });
        rotation = glm::rotate(rotation, glm::radians(_local_transform.rotation().y), glm::vec3{ 0.0F, 1.0F, 0.0F });
        rotation = glm::rotate(rotation, glm::radians(_local_transform.rotation().z), glm::vec3{ 0.0F, 0.0F, 1.0F });
        _data.direction = glm::xyz(rotation * glm::vec4(DIRECTION_ZERO, 1.0F));

        scene.set_frame_directional_light(_data);
    }

    template <>
    std::unique_ptr<directional_light_node> construct_node<directional_light_node>(
        std::string name,
        scene_node* parent,
        bool enabled)
    {
        return std::make_unique<directional_light_node>(MOVE(name), parent, enabled);
    }
} // namespace cathedral::engine
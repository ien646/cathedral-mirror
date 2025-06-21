#include "cathedral/engine/scene.hpp"

#include <cathedral/engine/nodes/directional_light_node.hpp>

namespace cathedral::engine
{
    glm::vec3 directional_light_node::position() const
    {
        return _data.position;
    }

    glm::vec3 directional_light_node::color() const
    {
        return _data.color;
    }

    float directional_light_node::intensity() const
    {
        return _data.intensity;
    }

    void directional_light_node::set_position(const glm::vec3& position)
    {
        _data.position = position;
    }

    void directional_light_node::set_color(const glm::vec3& color)
    {
        _data.color = color;
    }

    void directional_light_node::set_intensity(float intensity)
    {
        _data.intensity = intensity;
    }

    void directional_light_node::tick(scene& scene, double deltatime)
    {
        node::tick(scene, deltatime);
    }

    void directional_light_node::editor_tick(scene& scene, double deltatime)
    {
        node::editor_tick(scene, deltatime);
    }

    std::shared_ptr<scene_node> directional_light_node::copy(const std::string& copy_name, bool copy_children) const
    {
        auto result = std::make_shared<directional_light_node>(copy_name, _parent, !_disabled);

        result->_data = _data;
        if (copy_children)
        {
            copy_children_into(*result);
        }

        return result;
    }

    void directional_light_node::update_data(scene& scene) const
    {
        scene.set_frame_directional_light(_data);
    }
} // namespace cathedral::engine
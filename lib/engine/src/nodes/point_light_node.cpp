#include <cathedral/engine/nodes/point_light_node.hpp>

namespace cathedral::engine
{
    void point_light_node::set_position(const glm::vec3 pos)
    {
        set_local_position(pos);
        _data.position = world_position();
    }

    void point_light_node::set_color(const glm::vec3 color)
    {
        _data.color = color;
    }

    void point_light_node::set_insensity(const float intensity)
    {
        _data.intensity = intensity;
    }

    void point_light_node::set_range(const float range)
    {
        _data.range = range;
    }

    void point_light_node::set_falloff_coefficient(const float coefficient)
    {
        _data.falloff_coefficient = coefficient;
    }

    void point_light_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);
        _data.enabled = !_disabled;
    }

    const point_light_data& point_light_node::data() const
    {
        return _data;
    }

    std::shared_ptr<scene_node> point_light_node::copy(const std::string& copy_name, bool copy_children) const
    {
        auto result = std::make_shared<point_light_node>(copy_name, _parent, !_disabled);

        result->_data = _data;
        copy_children_into(*result);

        return result;
    }
} // namespace cathedral::engine
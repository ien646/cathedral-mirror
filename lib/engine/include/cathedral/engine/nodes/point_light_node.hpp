#pragma once

#include <cathedral/engine/lights.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class point_light_node : public node
    {
    public:
        using node::node;

        EXPORT_SCRIPT glm::vec3 position() const;
        EXPORT_SCRIPT glm::vec3 color() const;
        EXPORT_SCRIPT float intensity() const;
        EXPORT_SCRIPT float range() const;
        EXPORT_SCRIPT float falloff_coefficient() const;

        EXPORT_SCRIPT void set_position(glm::vec3 pos);
        EXPORT_SCRIPT void set_color(glm::vec3 color);
        EXPORT_SCRIPT void set_intensity(float intensity);
        EXPORT_SCRIPT void set_range(float range);
        EXPORT_SCRIPT void set_falloff_coefficient(float coefficient);

        void tick(scene& scene, double deltatime) override;

        void editor_tick(scene& scene, double deltatime) override;

        const point_light_data& data() const;

        std::unique_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

        constexpr node_type type() const override { return node_type::from_chars("CE::pntl"); }

    private:
        point_light_data _data = {};

        void update_data(scene& scene);
    };

    template <>
    std::unique_ptr<point_light_node> construct_node<point_light_node>(std::string name, scene_node* parent, bool enabled);
} // namespace cathedral::engine
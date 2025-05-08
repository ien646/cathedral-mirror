#pragma once

#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/point_light.hpp>

namespace cathedral::engine
{
    class point_light_node final : public node
    {
    public:
        using node::node;

        glm::vec3 position() const;
        glm::vec3 color() const;
        float intensity() const;
        float range() const;
        float falloff_coefficient() const;

        void set_position(glm::vec3 pos);
        void set_color(glm::vec3 color);
        void set_insensity(float intensity);
        void set_range(float range);
        void set_falloff_coefficient(float coefficient);

        void tick(scene& scene, double deltatime) override;

        const point_light_data& data() const;

        std::shared_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::POINT_LIGHT; }

    private:
        point_light_data _data = {};
    };
} // namespace cathedral::engine
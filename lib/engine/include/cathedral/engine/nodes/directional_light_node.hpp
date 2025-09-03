#pragma once

#include <cathedral/engine/lights.hpp>
#include <cathedral/engine/nodes/node.hpp>

namespace cathedral::engine
{
    class directional_light_node : public node
    {
    public:
        using node::node;

        glm::vec3 direction() const;
        glm::vec3 color() const;
        float intensity() const;

        void set_color(const glm::vec3& color);
        void set_intensity(float intensity);

        void tick(scene& scene, double deltatime) override;
        void editor_tick(scene& scene, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

        auto& data() { return _data; }

        const auto& data() const { return _data; }

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::DIRECTIONAL_LIGHT; }

    private:
        directional_light_data _data = {};

        void update_data(scene& scene);
    };

    template <>
    std::shared_ptr<directional_light_node> construct_node<directional_light_node>(
        std::string name,
        scene_node* parent,
        bool enabled);
} // namespace cathedral::engine
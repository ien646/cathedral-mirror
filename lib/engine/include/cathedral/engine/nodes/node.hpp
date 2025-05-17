#pragma once

#include <cathedral/engine/scene_node.hpp>
#include <cathedral/engine/transform.hpp>

namespace cathedral::engine
{
    class node : public scene_node
    {
    public:
        using scene_node::scene_node;

        glm::vec3 local_position() const;
        void set_local_position(glm::vec3 position);

        glm::vec3 local_rotation() const;
        void set_local_rotation(glm::vec3 rotation);

        glm::vec3 local_scale() const;
        void set_local_scale(glm::vec3 scale);

        glm::vec3 world_position() const;
        glm::vec3 world_scale() const;
        glm::vec3 world_rotation() const;

        void set_local_transform(const transform& tform);

        const transform& get_local_transform() const;

        const glm::mat4& get_world_model_matrix() const;

        void tick_setup(scene& scene) override;
        void tick(scene& scene, double deltatime) override;
        void editor_tick(scene& scene, double deltatime) override;

        std::shared_ptr<scene_node> copy(const std::string& copy_name, bool copy_children) const override;

        constexpr const char* typestr() const override { return typestr_from_type(type()); }

        constexpr node_type type() const override { return node_type::NODE; }

    protected:
        transform _local_transform;

        bool _world_model_needs_regen = true;
        void recalculate_world_model() const;

        void copy_children_into(scene_node& target) const;

    private:
        mutable glm::mat4 _world_model;
    };
} // namespace cathedral::engine
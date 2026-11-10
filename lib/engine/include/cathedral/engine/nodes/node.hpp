#pragma once

#include <cathedral/engine/scene_object.hpp>
#include <cathedral/engine/transform.hpp>

namespace cathedral::engine
{
    class node : public scene_object
    {
    public:
        using scene_object::scene_object;

        EXPORT_SCRIPT glm::vec3 local_position() const;
        EXPORT_SCRIPT void set_local_position(glm::vec3 position);

        EXPORT_SCRIPT glm::vec3 local_rotation() const;
        EXPORT_SCRIPT void set_local_rotation(glm::vec3 rotation);

        EXPORT_SCRIPT glm::vec3 local_scale() const;
        EXPORT_SCRIPT void set_local_scale(glm::vec3 scale);

        EXPORT_SCRIPT glm::vec3 world_position() const;
        EXPORT_SCRIPT glm::vec3 world_scale() const;
        EXPORT_SCRIPT glm::vec3 world_rotation() const;

        EXPORT_SCRIPT void set_local_transform(const transform& tform);
        EXPORT_SCRIPT const transform& local_transform() const;

        EXPORT_SCRIPT void translate(glm::vec3 translation);
        EXPORT_SCRIPT void rotate_degrees(glm::vec3 degrees);

        EXPORT_SCRIPT const glm::mat4& world_model_matrix() const;

        void tick_setup(scene& scene) override;
        void tick(scene& scene, double deltatime) override;
        void editor_tick(scene& scene, double deltatime) override;

        std::unique_ptr<scene_object> copy(const std::string& copy_name, bool copy_children) const override;

        EXPORT_SCRIPT constexpr node_type type() const override { return node_type::from_chars("CE::node"); }

    protected:
        transform _local_transform;

        void recalculate_world_model() const;

        void copy_children_into(scene_object& target) const;

        virtual void copy_into(node& target, bool copy_children) const;

    private:
        mutable glm::mat4 _world_model;
        mutable bool _world_model_needs_regen = true;
    };

    template <>
    std::unique_ptr<node> construct_node<node>(std::string name, scene_object* parent, bool enabled);
} // namespace cathedral::engine
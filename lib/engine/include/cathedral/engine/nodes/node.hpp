#pragma once

#include <cathedral/engine/transform.hpp>
#include <cathedral/engine/scene_node.hpp>

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

        const transform& get_local_transform() const;
        const transform& get_world_transform() const;

        virtual void tick(double deltatime) override;

    protected:
        transform _local_transform;
        mutable transform _world_transform;

        bool _world_transform_needs_refresh = true;
        void recalculate_world_transform() const;
    };
}
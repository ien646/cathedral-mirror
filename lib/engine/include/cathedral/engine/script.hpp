#pragma once

#include <cathedral/core.hpp>

#include <memory>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::engine, scene_node);

namespace cathedral::engine
{
    struct script
    {
        virtual ~script() = default;

        virtual void init(scene_node* node, scene& scene) = 0;

        virtual void tick(scene_node* node, scene& scene, double deltatime) = 0;

        virtual void editor_tick(scene_node* node, scene& scene, double deltatime) = 0;

        virtual void teardown(scene_node* node, scene& scene) = 0;

        virtual bool initialized() const = 0;

        virtual bool enabled() const = 0;

        virtual void set_enabled(bool enabled) = 0;
    };
} // namespace cathedral::engine
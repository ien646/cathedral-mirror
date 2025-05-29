#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::engine
{
    struct script
    {
        virtual ~script() = default;

        virtual void init(scene& scene) {}

        virtual void tick(scene& scene, double deltatime) {}

        virtual void editor_tick(scene& scene, double deltatime) {}

        virtual void teardown(scene& scene) {}

        virtual bool initialized() const = 0;

        virtual bool enabled() const = 0;

        virtual void set_enabled(bool enabled) = 0;
    };
} // namespace cathedral::engine
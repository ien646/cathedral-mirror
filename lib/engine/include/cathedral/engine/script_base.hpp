#pragma once

#include <cathedral/core.hpp>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::engine
{
    struct script_base
    {
        virtual ~script_base() = default;

        virtual void init(scene& scene) {}

        virtual void tick(scene& scene, double deltatime) {}

        virtual void editor_tick(scene& scene, double deltatime) {}

        virtual void teardown(scene& scene) {}
    };
} // namespace cathedral::engine
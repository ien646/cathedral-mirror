#pragma once

#include <cathedral/core.hpp>

namespace cathedral::engine
{
    class scene;

    class material : public uid_type
    {
    public:
        material(scene& scn)
            : _scene(scn)
        {
        }

        scene& get_scene() { return _scene; }

        virtual void update() = 0;

    protected:
        scene& _scene;
    };
} // namespace cathedral::engine
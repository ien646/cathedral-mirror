#pragma once

#include <cathedral/core.hpp>

namespace cathedral::engine
{
    class renderer;

    class material : public uid_type
    {
    public:
        material(renderer& scn)
            : _renderer(scn)
        {
        }

        renderer& get_renderer() { return _renderer; }

        virtual void update() = 0;

    protected:
        renderer& _renderer;
    };
} // namespace cathedral::engine
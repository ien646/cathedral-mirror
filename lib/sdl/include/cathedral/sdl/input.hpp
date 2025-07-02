#pragma once

#include <cathedral/sdl/keyboard.hpp>
#include <cathedral/sdl/mouse.hpp>

namespace cathedral::sdl
{
    class input
    {
    public:
        input();
        void tick();

    private:
        keyboard_input _keyboard;
        mouse_input _mouse;
    };
} // namespace cathedral::engine
#pragma once

#include <cathedral/sdl/window.hpp>

namespace cathedral::sdl
{
    void register_window(window& window);
    void unregister_window(const window& window);

    void global_poll_events();
}
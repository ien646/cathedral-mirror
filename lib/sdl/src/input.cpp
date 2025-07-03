#include <cathedral/sdl/input.hpp>

#include <SDL3/SDL.h>

namespace cathedral::sdl
{
    input::input()
        : _keyboard(*this)
        , _mouse(*this)
    {
        if (!SDL_Init(SDL_INIT_EVENTS))
        {
            CRITICAL_ERROR("Failed to initialize SDL events");
        }
    }

    void input::tick()
    {

    }
} // namespace cathedral::engine
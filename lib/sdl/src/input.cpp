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
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
                _keyboard.press_key(static_cast<keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_KEY_UP:
                _keyboard.release_key(static_cast<keyboard_keycode>(event.key.key));
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                _mouse.press_button(static_cast<mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                _mouse.release_button(static_cast<mouse_button>(event.button.button));
                break;
            case SDL_EVENT_MOUSE_MOTION:
                _mouse.set_mouse_position(glm::ivec2(event.motion.x, event.motion.y));
                _mouse.set_mouse_delta(glm::ivec2(event.motion.xrel, event.motion.yrel));
                break;
            default:
                break;
            }
        }
    }
} // namespace cathedral::engine
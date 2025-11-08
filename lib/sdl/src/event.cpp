#include <cathedral/sdl/event.hpp>

#include <cathedral/ds.hpp>
#include <cathedral/engine/input.hpp>

#include <print>
#include <unordered_map>

#include <SDL3/SDL_events.h>

namespace cathedral::sdl
{
    namespace
    {
        unordered_map<SDL_WindowID, window*> registered_windows;
    }

    void register_window(window& window)
    {
        registered_windows.emplace(SDL_GetWindowID(window.get_handle()), &window);
    }

    void unregister_window(const window& window)
    {
        registered_windows.erase(SDL_GetWindowID(window.get_handle()));
    }

    void global_poll_events()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                registered_windows.at(event.key.windowID)->handle_event(event);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                registered_windows.at(event.button.windowID)->handle_event(event);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                registered_windows.at(event.motion.windowID)->handle_event(event);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                registered_windows.at(event.window.windowID)->handle_event(event);
                break;
            case SDL_EVENT_TEXT_INPUT:
                registered_windows.at(event.text.windowID)->handle_event(event);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                registered_windows.at(event.wheel.windowID)->handle_event(event);
                break;
            default:
                break;
            }
        }
    }
} // namespace cathedral::sdl
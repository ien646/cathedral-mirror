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
            std::optional<SDL_WindowID> wid = {};
            switch (event.type)
            {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                wid = event.key.windowID;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                wid = event.button.windowID;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                wid = event.motion.windowID;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                wid = event.window.windowID;
                break;
            case SDL_EVENT_TEXT_INPUT:
            case SDL_EVENT_TEXT_EDITING:
                wid = event.text.windowID;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                wid = event.wheel.windowID;
                break;
            default:
                break;
            }

            if (wid.has_value())
            {
                registered_windows.at(*wid)->handle_event(event);
            }
        }
    }
} // namespace cathedral::sdl
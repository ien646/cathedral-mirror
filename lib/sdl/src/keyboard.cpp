#include <cathedral/sdl/keyboard.hpp>

#include <cathedral/sdl/input.hpp>

#include <SDL3/SDL.h>

namespace cathedral::sdl
{
    keyboard_input::keyboard_input(input& input)
        : _input(input)
    {
    }

    bool keyboard_input::is_key_pressed(const keyboard_keycode k)
    {
        return _pressed_keys.contains(k);
    }

    bool keyboard_input::is_key_just_pressed(const keyboard_keycode k)
    {
        return _just_pressed_keys.contains(k);
    }

    bool keyboard_input::is_key_just_released(const keyboard_keycode k)
    {
        return _just_released_keys.contains(k);
    }

    void keyboard_input::tick()
    {
        _input.tick();
    }

    void keyboard_input::press_key(const keyboard_keycode k)
    {
        _pressed_keys.insert(k);
        _just_pressed_keys.insert(k);
        _just_released_keys.erase(k);
    }

    void keyboard_input::release_key(const keyboard_keycode k)
    {
        _pressed_keys.erase(k);
        _just_pressed_keys.erase(k);
        _just_released_keys.insert(k);
    }
} // namespace cathedral::sdl
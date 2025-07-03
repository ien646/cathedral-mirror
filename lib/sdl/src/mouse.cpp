#include <cathedral/sdl/mouse.hpp>

#include <cathedral/sdl/input.hpp>

namespace cathedral::sdl
{
    mouse_input::mouse_input(input& input)
        : _input(input)
    {
    }

    bool mouse_input::is_button_pressed(const mouse_button b)
    {
        return _pressed_buttons.contains(b);
    }

    bool mouse_input::is_button_just_pressed(const mouse_button b)
    {
        return _just_pressed_buttons.contains(b);
    }

    bool mouse_input::is_button_just_released(const mouse_button b)
    {
        return _just_released_buttons.contains(b);
    }

    glm::ivec2 mouse_input::delta()
    {
        return _delta;
    }

    glm::ivec2 mouse_input::position()
    {
        return _position;
    }

    void mouse_input::tick()
    {
        _input.tick();
    }

    void mouse_input::press_button(const mouse_button b)
    {
        _pressed_buttons.insert(b);
        _just_pressed_buttons.insert(b);
        _just_released_buttons.erase(b);
    }

    void mouse_input::release_button(const mouse_button b)
    {
        _pressed_buttons.erase(b);
        _just_pressed_buttons.erase(b);
        _just_released_buttons.insert(b);
    }

    void mouse_input::set_mouse_delta(const glm::ivec2 delta)
    {
        _delta = delta;
    }

    void mouse_input::set_mouse_position(const glm::ivec2 pos)
    {
        _position = pos;
    }
} // namespace cathedral::engine
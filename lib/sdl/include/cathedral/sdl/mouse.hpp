#pragma once

#include <cathedral/ds.hpp>
#include <cathedral/engine/input.hpp>

FORWARD_CLASS(cathedral::sdl, input);

namespace cathedral::sdl
{
    using mouse_button = engine::mouse_button;

    class mouse_input final : public engine::mouse_input_interface
    {
    public:
        bool is_button_pressed(mouse_button) override;
        bool is_button_just_pressed(mouse_button) override;
        bool is_button_just_released(mouse_button) override;
        glm::ivec2 delta() override;
        glm::ivec2 position() override;
        void tick() override;

        void press_button(mouse_button);
        void release_button(mouse_button);
        void set_mouse_delta(glm::ivec2);
        void set_mouse_position(glm::ivec2);

    private:
        unordered_set<mouse_button> _pressed_buttons;
        unordered_set<mouse_button> _just_pressed_buttons;
        unordered_set<mouse_button> _just_released_buttons;
        glm::ivec2 _position = {};
        glm::ivec2 _delta = {};
    };
} // namespace cathedral::sdl
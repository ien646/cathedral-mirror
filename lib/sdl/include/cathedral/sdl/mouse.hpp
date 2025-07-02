#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/input.hpp>

#include <unordered_set>

FORWARD_CLASS(cathedral::sdl, input);

namespace cathedral::sdl
{
    using mouse_button = engine::mouse_button;

    class mouse_input final : public engine::mouse_input_interface
    {
    public:
        explicit mouse_input(input& input);
        bool is_mouse_button_pressed(mouse_button) override;
        bool is_mouse_button_just_pressed(mouse_button) override;
        bool is_mouse_button_just_released(mouse_button) override;
        glm::ivec2 get_mouse_delta() override;
        glm::ivec2 get_mouse_position() override;
        void tick() override;

        void press_button(mouse_button);
        void release_button(mouse_button);
        void set_mouse_delta(glm::ivec2);
        void set_mouse_position(glm::ivec2);

    private:
        input& _input;
        std::unordered_set<mouse_button> _pressed_buttons;
        std::unordered_set<mouse_button> _just_pressed_buttons;
        std::unordered_set<mouse_button> _just_released_buttons;
        glm::ivec2 _position = {};
        glm::ivec2 _delta = {};
    };
} // namespace cathedral::sdl
#pragma once

#include <cathedral/core.hpp>
#include <cathedral/engine/input.hpp>

#include <unordered_set>

FORWARD_CLASS(cathedral::sdl, input);

namespace cathedral::sdl
{
    using keyboard_keycode = engine::keyboard_keycode;

    class keyboard_input final : public engine::keyboard_input_interface
    {
    public:
        bool is_key_pressed(keyboard_keycode) override;
        bool is_key_just_pressed(keyboard_keycode) override;
        bool is_key_just_released(keyboard_keycode) override;
        void tick() override;

        void press_key(keyboard_keycode);
        void release_key(keyboard_keycode);;

    private:
        std::unordered_set<keyboard_keycode> _pressed_keys;
        std::unordered_set<keyboard_keycode> _just_pressed_keys;
        std::unordered_set<keyboard_keycode> _just_released_keys;
    };
}
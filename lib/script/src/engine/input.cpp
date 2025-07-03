#include "cathedral/engine/input.hpp"

#include "cathedral/script/init_macros.hpp"

#include <cathedral/script/engine/input.hpp>

constexpr auto ANNOTATIONS = R"lua(

---@class keyboard_input_interface
---@field public is_key_pressed fun(k: keyboard_keycode): boolean
---@field public is_key_just_pressed fun(k: keyboard_keycode): boolean
---@field public is_key_just_released fun(k: keyboard_keycode): boolean
local keyboard_input_interface = {}

---@class mouse_input_interface
---@field public is_button_pressed fun(b: mouse_button): boolean
---@field public is_button_just_pressed fun(b: mouse_button): boolean
---@field public is_button_just_released fun(b: mouse_button): boolean
---@field public position fun(): ivec2
---@field public delta fun(): ivec2
local mouse_input_interface = {}
)lua";

namespace cathedral::script::engine
{
    void input_initializer::initialize(state& s)
    {
        AUTO_INIT_ENUM(s, cathedral::engine, keyboard_keycode);
        AUTO_INIT_ENUM(s, cathedral::engine, mouse_button);

        { // Keyboard
            AUTO_INIT_NEW_TYPE(s, cathedral::engine, keyboard_input_interface);
            AUTO_FUNC(is_key_pressed);
            AUTO_FUNC(is_key_just_pressed);
            AUTO_FUNC(is_key_just_released);
        }
        { // Mouse
            AUTO_INIT_NEW_TYPE(s, cathedral::engine, mouse_input_interface);
            AUTO_FUNC(is_button_pressed);
            AUTO_FUNC(is_button_just_pressed);
            AUTO_FUNC(is_button_just_released);
            AUTO_FUNC(position);
            AUTO_FUNC(delta);
        }
    }

    const std::string& input_initializer::get_annotations()
    {
        static const std::string annotations = ANNOTATIONS;
        return annotations;
    }
} // namespace cathedral::script::engine
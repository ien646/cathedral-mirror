#include <cathedral/script/engine/input.hpp>

#include <cathedral/engine/input.hpp>
#include <cathedral/script/init_macros.hpp>

const std::string annotations = R"lua(

---@class keyboard_input_interface
---@field public is_key_pressed fun(self, k: number): boolean
---@field public is_key_just_pressed fun(self, k: number): boolean
---@field public is_key_just_released fun(self, k: number): boolean
keyboard_input_interface = {}

---@class mouse_input_interface
---@field public is_button_pressed fun(self, b: number): boolean
---@field public is_button_just_pressed fun(self, b: number): boolean
---@field public is_button_just_released fun(self, b: number): boolean
---@field public position fun(self): ivec2
---@field public delta fun(self): ivec2
mouse_input_interface = {}
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
        return annotations;
    }
} // namespace cathedral::script::engine
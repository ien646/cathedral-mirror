#include <cathedral/script/engine/input.hpp>

#include <cathedral/engine/input.hpp>

#include <cathedral/script/global_state.hpp>
#include <cathedral/script/helpers.hpp>

namespace cathedral::script
{
    void register_input(global_state& state)
    {
        register_global_enum<engine::keyboard_keycode>(state, "Key");
        register_global_enum<engine::mouse_button>(state, "MouseButton");
        register_class_auto<engine::keyboard_input_interface>(state, "KeyboardInput");
        register_class_auto<engine::mouse_input_interface>(state, "MouseInput");
    }
} // namespace cathedral::script
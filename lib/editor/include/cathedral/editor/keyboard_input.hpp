#pragma once

#include <cathedral/engine/input.hpp>

#include <QObject>

#include <unordered_set>

namespace cathedral::editor
{
    class editor_keyboard_input final
        : public engine::keyboard_input_interface
        , public QObject
    {
    public:
        explicit editor_keyboard_input(QObject* parent);
        bool is_key_pressed(engine::keyboard_keycode) override;
        bool is_key_just_pressed(engine::keyboard_keycode) override;
        bool is_key_just_released(engine::keyboard_keycode) override;
        void tick() override;

        void press_key(Qt::KeyboardModifiers mods, Qt::Key);
        void release_key(Qt::KeyboardModifiers mods, Qt::Key);

    private:
        std::unordered_set<engine::keyboard_keycode> _pressed_keys;
        std::unordered_set<engine::keyboard_keycode> _just_pressed_keys;
        std::unordered_set<engine::keyboard_keycode> _just_released_keys;

        bool eventFilter(QObject* obj, QEvent* ev) override;
    };
} // namespace cathedral::editor
#pragma once

#include <cathedral/engine/input.hpp>

#include <QObject>
#include <unordered_set>

namespace cathedral::editor
{
    class editor_mouse_input final
        : public engine::mouse_input_interface
        , public QObject
    {
    public:
        explicit editor_mouse_input(QObject* parent);
        bool is_mouse_button_pressed(engine::mouse_button) override;
        bool is_mouse_button_just_pressed(engine::mouse_button) override;
        bool is_mouse_button_just_released(engine::mouse_button) override;
        glm::ivec2 get_mouse_delta() override;
        glm::ivec2 get_mouse_position() override;
        void set_mouse_delta(glm::ivec2 mouse_delta);
        void set_mouse_position(glm::ivec2 mouse_position);

        void tick() override;

        void press_button(Qt::MouseButton);
        void release_button(Qt::MouseButton);

    private:
        std::unordered_set<engine::mouse_button> _pressed_buttons;
        std::unordered_set<engine::mouse_button> _just_pressed_buttons;
        std::unordered_set<engine::mouse_button> _just_released_buttons;

        glm::ivec2 _mouse_delta = {};
        glm::ivec2 _last_mouse_position = {};
        glm::ivec2 _mouse_position = {};

        bool eventFilter(QObject* obj, QEvent* event) override;
    };
} // namespace cathedral::editor
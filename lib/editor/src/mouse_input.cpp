#include <QEvent>
#include <QMouseEvent>
#include <cathedral/editor/mouse_input.hpp>

namespace cathedral::editor
{
    namespace
    {
        engine::mouse_button qt_mouse_button_to_engine(const Qt::MouseButton button)
        {
            switch (button)
            {
            case Qt::LeftButton:
                return engine::mouse_button::BUTTON_LEFT;
            case Qt::RightButton:
                return engine::mouse_button::BUTTON_RIGHT;
            case Qt::MiddleButton:
                return engine::mouse_button::BUTTON_MIDDLE;
            case Qt::XButton1:
                return engine::mouse_button::BUTTON_X1;
            case Qt::XButton2:
                return engine::mouse_button::BUTTON_X2;
            default:
                return engine::mouse_button::NONE;
            }
        }
    } // namespace

    editor_mouse_input::editor_mouse_input(QObject* parent)
        : QObject(parent)
    {
    }

    bool editor_mouse_input::is_mouse_button_pressed(const engine::mouse_button b)
    {
        return _pressed_buttons.contains(b);
    }

    bool editor_mouse_input::is_mouse_button_just_pressed(const engine::mouse_button b)
    {
        return _just_pressed_buttons.contains(b);
    }

    bool editor_mouse_input::is_mouse_button_just_released(const engine::mouse_button b)
    {
        return _just_released_buttons.contains(b);
    }

    glm::ivec2 editor_mouse_input::get_mouse_delta()
    {
        return _mouse_delta;
    }

    glm::ivec2 editor_mouse_input::get_mouse_position()
    {
        return _mouse_position;
    }

    void editor_mouse_input::tick()
    {
        _just_pressed_buttons.clear();
        _just_released_buttons.clear();
    }

    void editor_mouse_input::press_button(const Qt::MouseButton b)
    {
        const auto emb = qt_mouse_button_to_engine(b);
        _just_pressed_buttons.insert(emb);
        _just_released_buttons.erase(emb);
        _pressed_buttons.insert(emb);
    }

    void editor_mouse_input::release_button(const Qt::MouseButton b)
    {
        const auto emb = qt_mouse_button_to_engine(b);
        _just_pressed_buttons.erase(emb);
        _just_released_buttons.insert(emb);
        _pressed_buttons.erase(emb);
    }

    bool editor_mouse_input::eventFilter(QObject* obj, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            press_button(dynamic_cast<QMouseEvent*>(event)->button());
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            release_button(dynamic_cast<QMouseEvent*>(event)->button());
        }
        else if (event->type() == QEvent::MouseMove)
        {
            const QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
            _last_mouse_position = _mouse_position;
            _mouse_position = { mouseEvent->pos().x(), mouseEvent->pos().y() };
            _mouse_delta = _mouse_position - _last_mouse_position;
        }
        return QObject::eventFilter(obj, event);
    }
} // namespace cathedral::editor
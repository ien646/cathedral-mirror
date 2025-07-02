#include <QApplication>
#include <QKeyEvent>
#include <cathedral/editor/keyboard_input.hpp>

#include <cathedral/core.hpp>

namespace cathedral::editor
{
    namespace
    {
        engine::keyboard_keycode qt_key_to_engine(const Qt::KeyboardModifiers mods, const Qt::Key key)
        {
            if (mods & Qt::KeyboardModifier::KeypadModifier)
            {
                switch (key)
                {
                case Qt::Key_0:
                    return engine::keyboard_keycode::NUMPAD_0;
                case Qt::Key_1:
                    return engine::keyboard_keycode::NUMPAD_1;
                case Qt::Key_2:
                    return engine::keyboard_keycode::NUMPAD_2;
                case Qt::Key_3:
                    return engine::keyboard_keycode::NUMPAD_3;
                case Qt::Key_4:
                    return engine::keyboard_keycode::NUMPAD_4;
                case Qt::Key_5:
                    return engine::keyboard_keycode::NUMPAD_5;
                case Qt::Key_6:
                    return engine::keyboard_keycode::NUMPAD_6;
                case Qt::Key_7:
                    return engine::keyboard_keycode::NUMPAD_7;
                case Qt::Key_8:
                    return engine::keyboard_keycode::NUMPAD_8;
                case Qt::Key_9:
                    return engine::keyboard_keycode::NUMPAD_9;
                case Qt::Key_division:
                    return engine::keyboard_keycode::NUMPAD_DIVIDE;
                case Qt::Key_multiply:
                    return engine::keyboard_keycode::NUMPAD_MULTIPLY;
                case Qt::Key_Minus:
                    return engine::keyboard_keycode::NUMPAD_MINUS;
                case Qt::Key_Plus:
                    return engine::keyboard_keycode::NUMPAD_PLUS;
                case Qt::Key_Enter:
                    return engine::keyboard_keycode::NUMPAD_ENTER;
                case Qt::Key_Period:
                    return engine::keyboard_keycode::NUMPAD_PERIOD;
                default:
                    log_error(
                        std::format("Unhandled qt keycode '{}'. Cannot convert to engine keycode", static_cast<int>(key)));
                    return engine::keyboard_keycode::NONE;
                }
            }

            switch (key)
            {
            case Qt::Key_0:
                return engine::keyboard_keycode::_0;
            case Qt::Key_1:
                return engine::keyboard_keycode::_1;
            case Qt::Key_2:
                return engine::keyboard_keycode::_2;
            case Qt::Key_3:
                return engine::keyboard_keycode::_3;
            case Qt::Key_4:
                return engine::keyboard_keycode::_4;
            case Qt::Key_5:
                return engine::keyboard_keycode::_5;
            case Qt::Key_6:
                return engine::keyboard_keycode::_6;
            case Qt::Key_7:
                return engine::keyboard_keycode::_7;
            case Qt::Key_8:
                return engine::keyboard_keycode::_8;
            case Qt::Key_9:

                return engine::keyboard_keycode::_9;
            case Qt::Key_A:
                return engine::keyboard_keycode::A;
            case Qt::Key_B:
                return engine::keyboard_keycode::B;
            case Qt::Key_C:
                return engine::keyboard_keycode::C;
            case Qt::Key_D:
                return engine::keyboard_keycode::D;
            case Qt::Key_E:
                return engine::keyboard_keycode::E;
            case Qt::Key_F:
                return engine::keyboard_keycode::F;
            case Qt::Key_G:
                return engine::keyboard_keycode::G;
            case Qt::Key_H:
                return engine::keyboard_keycode::H;
            case Qt::Key_I:
                return engine::keyboard_keycode::I;
            case Qt::Key_J:
                return engine::keyboard_keycode::J;
            case Qt::Key_K:
                return engine::keyboard_keycode::K;
            case Qt::Key_L:
                return engine::keyboard_keycode::L;
            case Qt::Key_M:
                return engine::keyboard_keycode::M;
            case Qt::Key_N:
                return engine::keyboard_keycode::N;
            case Qt::Key_O:
                return engine::keyboard_keycode::O;
            case Qt::Key_P:
                return engine::keyboard_keycode::P;
            case Qt::Key_Q:
                return engine::keyboard_keycode::Q;
            case Qt::Key_R:
                return engine::keyboard_keycode::R;
            case Qt::Key_S:
                return engine::keyboard_keycode::S;
            case Qt::Key_T:
                return engine::keyboard_keycode::T;
            case Qt::Key_U:
                return engine::keyboard_keycode::U;
            case Qt::Key_V:
                return engine::keyboard_keycode::V;
            case Qt::Key_W:
                return engine::keyboard_keycode::W;
            case Qt::Key_X:
                return engine::keyboard_keycode::X;
            case Qt::Key_Y:
                return engine::keyboard_keycode::Y;
            case Qt::Key_Z:

                return engine::keyboard_keycode::Z;
            case Qt::Key_Left:
                return engine::keyboard_keycode::LEFT;
            case Qt::Key_Right:
                return engine::keyboard_keycode::RIGHT;
            case Qt::Key_Up:
                return engine::keyboard_keycode::UP;
            case Qt::Key_Down:
                return engine::keyboard_keycode::DOWN;

            case Qt::Key_F1:
                return engine::keyboard_keycode::F1;
            case Qt::Key_F2:
                return engine::keyboard_keycode::F2;
            case Qt::Key_F3:
                return engine::keyboard_keycode::F3;
            case Qt::Key_F4:
                return engine::keyboard_keycode::F4;
            case Qt::Key_F5:
                return engine::keyboard_keycode::F5;
            case Qt::Key_F6:
                return engine::keyboard_keycode::F6;
            case Qt::Key_F7:
                return engine::keyboard_keycode::F7;
            case Qt::Key_F8:
                return engine::keyboard_keycode::F8;
            case Qt::Key_F9:
                return engine::keyboard_keycode::F9;
            case Qt::Key_F10:
                return engine::keyboard_keycode::F10;
            case Qt::Key_F11:
                return engine::keyboard_keycode::F11;
            case Qt::Key_F12:
                return engine::keyboard_keycode::F12;

            case Qt::Key_Delete:
                return engine::keyboard_keycode::DELETE;
            case Qt::Key_Insert:
                return engine::keyboard_keycode::INSERT;
            case Qt::Key_Home:
                return engine::keyboard_keycode::HOME;
            case Qt::Key_PageUp:
                return engine::keyboard_keycode::PAGEUP;
            case Qt::Key_End:
                return engine::keyboard_keycode::END;
            case Qt::Key_PageDown:
                return engine::keyboard_keycode::PAGEDOWN;

            case Qt::Key_NumLock:
                return engine::keyboard_keycode::NUMLOCKCLEAR;
            case Qt::Key_Return:
                return engine::keyboard_keycode::RETURN;
            case Qt::Key_Escape:
                return engine::keyboard_keycode::ESCAPE;
            case Qt::Key_Backspace:
                return engine::keyboard_keycode::BACKSPACE;
            case Qt::Key_Tab:
                return engine::keyboard_keycode::TAB;
            case Qt::Key_Space:
                return engine::keyboard_keycode::SPACE;
            case Qt::Key_Exclam:
                return engine::keyboard_keycode::EXCLAMATION;
            case Qt::Key_QuoteDbl:
                return engine::keyboard_keycode::DOUBLE_QUOTE;
            case Qt::Key_NumberSign:
                return engine::keyboard_keycode::HASH;
            case Qt::Key_Dollar:
                return engine::keyboard_keycode::DOLLAR;
            case Qt::Key_Percent:
                return engine::keyboard_keycode::PERCENT;
            case Qt::Key_Ampersand:
                return engine::keyboard_keycode::AMPERSAND;
            case Qt::Key_Apostrophe:
                return engine::keyboard_keycode::APOSTROPHE;
            case Qt::Key_ParenLeft:
                return engine::keyboard_keycode::LEFT_PARENTHESIS;
            case Qt::Key_ParenRight:
                return engine::keyboard_keycode::RIGHT_PARENTHESIS;
            case Qt::Key_Asterisk:
                return engine::keyboard_keycode::ASTERISK;
            case Qt::Key_Plus:
                return engine::keyboard_keycode::PLUS;
            case Qt::Key_Comma:
                return engine::keyboard_keycode::COMMA;
            case Qt::Key_Minus:
                return engine::keyboard_keycode::MINUS;
            case Qt::Key_Period:
                return engine::keyboard_keycode::PERIOD;
            case Qt::Key_Slash:
                return engine::keyboard_keycode::SLASH;
            case Qt::Key_Colon:
                return engine::keyboard_keycode::COLON;
            case Qt::Key_Semicolon:
                return engine::keyboard_keycode::SEMICOLON;
            case Qt::Key_Less:
                return engine::keyboard_keycode::LESS;
            case Qt::Key_Equal:
                return engine::keyboard_keycode::EQUALS;
            case Qt::Key_Greater:
                return engine::keyboard_keycode::GREATER;
            case Qt::Key_Question:
                return engine::keyboard_keycode::QUESTION;
            case Qt::Key_At:
                return engine::keyboard_keycode::AT;
            case Qt::Key_BracketLeft:
                return engine::keyboard_keycode::LEFT_BRACKET;
            case Qt::Key_Backslash:
                return engine::keyboard_keycode::BACKSLASH;
            case Qt::Key_BracketRight:
                return engine::keyboard_keycode::RIGHT_BRACKET;
            case Qt::Key_AsciiCircum:
                return engine::keyboard_keycode::CARET;
            case Qt::Key_Underscore:
                return engine::keyboard_keycode::UNDERSCORE;
            case Qt::Key_Dead_Grave:
                return engine::keyboard_keycode::GRAVE_ACCENT;
            case Qt::Key_BraceLeft:
                return engine::keyboard_keycode::LEFT_BRACE;
            case Qt::Key_Bar:
                return engine::keyboard_keycode::PIPE;
            case Qt::Key_BraceRight:
                return engine::keyboard_keycode::RIGHT_BRACE;
            case Qt::Key_AsciiTilde:
                return engine::keyboard_keycode::TILDE;
            case Qt::Key_plusminus:
                return engine::keyboard_keycode::PLUSMINUS;
            case Qt::Key_CapsLock:
                return engine::keyboard_keycode::CAPSLOCK;

            default:
                log_error(std::format("Unhandled qt keycode '{}'. Cannot convert to engine keycode", static_cast<int>(key)));
                return engine::keyboard_keycode::NONE;
            }
        }
    } // namespace

    editor_keyboard_input::editor_keyboard_input(QObject* parent)
        : QObject(parent)
    {
        QApplication::instance()->installEventFilter(this);
    }

    bool editor_keyboard_input::is_key_pressed(const engine::keyboard_keycode k)
    {
        return _pressed_keys.contains(k);
    }

    bool editor_keyboard_input::is_key_just_pressed(const engine::keyboard_keycode k)
    {
        return _just_pressed_keys.contains(k);
    }

    bool editor_keyboard_input::is_key_just_released(const engine::keyboard_keycode k)
    {
        return _just_released_keys.contains(k);
    }

    void editor_keyboard_input::tick()
    {
        _just_pressed_keys.clear();
        _just_released_keys.clear();
    }

    void editor_keyboard_input::press_key(const Qt::KeyboardModifiers mods, const Qt::Key key)
    {
        const engine::keyboard_keycode ekc = qt_key_to_engine(mods, key);
        _just_pressed_keys.insert(ekc);
        _just_released_keys.erase(ekc);
        _pressed_keys.insert(ekc);
    }

    void editor_keyboard_input::release_key(const Qt::KeyboardModifiers mods, const Qt::Key key)
    {
        const engine::keyboard_keycode ekc = qt_key_to_engine(mods, key);
        _just_pressed_keys.erase(ekc);
        _just_released_keys.insert(ekc);
        _pressed_keys.erase(ekc);
    }

    bool editor_keyboard_input::eventFilter(QObject* obj, QEvent* ev)
    {
        if (ev->type() == QEvent::KeyPress)
        {
            const auto key_event = dynamic_cast<QKeyEvent*>(ev);

            if (!key_event->isAutoRepeat())
            {
                press_key(key_event->modifiers(), static_cast<Qt::Key>(key_event->key()));

                const auto mods = key_event->modifiers();

                const auto press_modifier = [this](const engine::keyboard_keycode k) {
                    _pressed_keys.insert(k);
                    _just_pressed_keys.insert(k);
                    _just_released_keys.erase(k);
                };

                if (mods & Qt::ControlModifier)
                {
                    press_modifier(engine::keyboard_keycode::CONTROL);
                }
                if (mods & Qt::ShiftModifier)
                {
                    press_modifier(engine::keyboard_keycode::SHIFT);
                }
                if (mods & Qt::AltModifier)
                {
                    press_modifier(engine::keyboard_keycode::ALT);
                }
            }
        }
        else if (ev->type() == QEvent::KeyRelease)
        {
            const auto key_event = dynamic_cast<QKeyEvent*>(ev);
            release_key(key_event->modifiers(), static_cast<Qt::Key>(key_event->key()));

            const auto release_modifier = [this](const engine::keyboard_keycode k) {
                _pressed_keys.erase(k);
                _just_pressed_keys.erase(k);
                _just_released_keys.insert(k);
            };

            const auto mods = key_event->modifiers();

            if (mods & Qt::ControlModifier)
            {
                release_modifier(engine::keyboard_keycode::CONTROL);
            }
            if (mods & Qt::ShiftModifier)
            {
                release_modifier(engine::keyboard_keycode::SHIFT);
            }
            if (mods & Qt::AltModifier)
            {
                release_modifier(engine::keyboard_keycode::ALT);
            }
        }
        return QObject::eventFilter(obj, ev);
    }
} // namespace cathedral::editor
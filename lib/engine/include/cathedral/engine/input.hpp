#pragma once

#include <glm/vec2.hpp>

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

namespace cathedral::engine
{
    // Keycodes are SDL keycodes for compatibility
    // Implementations must convert keycodes appropriately
    enum class keyboard_keycode : SDL_Keycode
    {
        NONE = 0,
        _0 = SDLK_0,
        _1 = SDLK_1,
        _2 = SDLK_2,
        _3 = SDLK_3,
        _4 = SDLK_4,
        _5 = SDLK_5,
        _6 = SDLK_6,
        _7 = SDLK_7,
        _8 = SDLK_8,
        _9 = SDLK_9,

        A = SDLK_A,
        B = SDLK_B,
        C = SDLK_C,
        D = SDLK_D,
        E = SDLK_E,
        F = SDLK_F,
        G = SDLK_G,
        H = SDLK_H,
        I = SDLK_I,
        J = SDLK_J,
        K = SDLK_K,
        L = SDLK_L,
        M = SDLK_M,
        N = SDLK_N,
        O = SDLK_O,
        P = SDLK_P,
        Q = SDLK_Q,
        R = SDLK_R,
        S = SDLK_S,
        T = SDLK_T,
        U = SDLK_U,
        V = SDLK_V,
        W = SDLK_W,
        X = SDLK_X,
        Y = SDLK_Y,
        Z = SDLK_Z,

        F1 = SDLK_F1,
        F2 = SDLK_F2,
        F3 = SDLK_F3,
        F4 = SDLK_F4,
        F5 = SDLK_F5,
        F6 = SDLK_F6,
        F7 = SDLK_F7,
        F8 = SDLK_F8,
        F9 = SDLK_F9,
        F10 = SDLK_F10,
        F11 = SDLK_F11,
        F12 = SDLK_F12,

        DELETE = SDLK_DELETE,
        INSERT = SDLK_INSERT,
        HOME = SDLK_HOME,
        PAGEUP = SDLK_PAGEUP,
        END = SDLK_END,
        PAGEDOWN = SDLK_PAGEDOWN,

        RIGHT = SDLK_RIGHT,
        LEFT = SDLK_LEFT,
        DOWN = SDLK_DOWN,
        UP = SDLK_UP,

        NUMLOCKCLEAR = SDLK_NUMLOCKCLEAR,
        NUMPAD_DIVIDE = SDLK_KP_DIVIDE,
        NUMPAD_MULTIPLY = SDLK_KP_MULTIPLY,
        NUMPAD_MINUS = SDLK_KP_MINUS,
        NUMPAD_PLUS = SDLK_KP_PLUS,
        NUMPAD_ENTER = SDLK_KP_ENTER,
        NUMPAD_1 = SDLK_KP_1,
        NUMPAD_2 = SDLK_KP_2,
        NUMPAD_3 = SDLK_KP_3,
        NUMPAD_4 = SDLK_KP_4,
        NUMPAD_5 = SDLK_KP_5,
        NUMPAD_6 = SDLK_KP_6,
        NUMPAD_7 = SDLK_KP_7,
        NUMPAD_8 = SDLK_KP_8,
        NUMPAD_9 = SDLK_KP_9,
        NUMPAD_0 = SDLK_KP_0,
        NUMPAD_PERIOD = SDLK_KP_PERIOD,

        RETURN = SDLK_RETURN,
        ESCAPE = SDLK_ESCAPE,
        BACKSPACE = SDLK_BACKSPACE,
        TAB = SDLK_TAB,
        SPACE = SDLK_SPACE,
        EXCLAMATION = SDLK_EXCLAIM,
        DOUBLE_QUOTE = SDLK_DBLAPOSTROPHE,
        HASH = SDLK_HASH,
        DOLLAR = SDLK_DOLLAR,
        PERCENT = SDLK_PERCENT,
        AMPERSAND = SDLK_AMPERSAND,
        APOSTROPHE = SDLK_APOSTROPHE,
        LEFT_PARENTHESIS = SDLK_LEFTPAREN,
        RIGHT_PARENTHESIS = SDLK_RIGHTPAREN,
        ASTERISK = SDLK_ASTERISK,
        PLUS = SDLK_PLUS,
        COMMA = SDLK_COMMA,
        MINUS = SDLK_MINUS,
        PERIOD = SDLK_PERIOD,
        SLASH = SDLK_SLASH,
        COLON = SDLK_COLON,
        SEMICOLON = SDLK_SEMICOLON,
        LESS = SDLK_LESS,
        EQUALS = SDLK_EQUALS,
        GREATER = SDLK_GREATER,
        QUESTION = SDLK_QUESTION,
        AT = SDLK_AT,
        LEFT_BRACKET = SDLK_LEFTBRACKET,
        BACKSLASH = SDLK_BACKSLASH,
        RIGHT_BRACKET = SDLK_RIGHTBRACKET,
        CARET = SDLK_CARET,
        UNDERSCORE = SDLK_UNDERSCORE,
        GRAVE_ACCENT = SDLK_GRAVE,
        LEFT_BRACE = SDLK_LEFTBRACE,
        PIPE = SDLK_PIPE,
        RIGHT_BRACE = SDLK_RIGHTBRACE,
        TILDE = SDLK_TILDE,
        PLUSMINUS = SDLK_PLUSMINUS,
        CAPSLOCK = SDLK_CAPSLOCK,

        SHIFT,
        CONTROL,
        ALT
    };

    enum class mouse_button : uint8_t
    {
        NONE = 0,
        BUTTON_LEFT = SDL_BUTTON_LEFT,
        BUTTON_RIGHT = SDL_BUTTON_RIGHT,
        BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
        BUTTON_X1 = SDL_BUTTON_X1,
        BUTTON_X2 = SDL_BUTTON_X2
    };

    struct keyboard_input_interface
    {
        virtual ~keyboard_input_interface() = default;

        virtual bool is_key_pressed(keyboard_keycode) = 0;
        virtual bool is_key_just_pressed(keyboard_keycode) = 0;
        virtual bool is_key_just_released(keyboard_keycode) = 0;

        virtual void tick() = 0;
    };

    struct mouse_input_interface
    {
        virtual ~mouse_input_interface() = default;

        virtual bool is_mouse_button_pressed(mouse_button) = 0;
        virtual bool is_mouse_button_just_pressed(mouse_button) = 0;
        virtual bool is_mouse_button_just_released(mouse_button) = 0;
        virtual glm::ivec2 get_mouse_delta() = 0;
        virtual glm::ivec2 get_mouse_position() = 0;

        virtual void tick() = 0;
    };
} // namespace cathedral::engine
#pragma once

#include <variant>
#include <string>

#include "core/Types.hpp"

namespace ui {

// -- Mouse events --

struct MouseEvent {
    enum Type { Down, Up, Move, Enter, Leave } type;
    Point globalPos;
    Point localPos;
    int button = 0;   // GLFW_MOUSE_BUTTON_1, etc.
    int mods = 0;     // GLFW_MOD_SHIFT, etc.
};

struct ScrollEvent {
    Point globalPos;
    float dx = 0;
    float dy = 0;
};

// -- Keyboard events --

struct KeyEvent {
    enum Type { Down, Up, Repeat } type;
    int key = 0;       // GLFW_KEY_*
    int scancode = 0;
    int mods = 0;
};

struct CharEvent {
    uint32_t codepoint = 0;
};

// -- Window events --

struct ResizeEvent {
    Size newSize;
};

// -- Focus events --

struct FocusEvent {
    enum Type { Gained, Lost } type;
};

// -- Unified event type --

using Event = std::variant<
    MouseEvent,
    ScrollEvent,
    KeyEvent,
    CharEvent,
    ResizeEvent,
    FocusEvent
>;

} // namespace ui

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

// Rich keyboard event with named editing operations (EUI-NEO pattern)
struct KeyEvent {
    enum Type { Down, Up, Repeat } type;
    int key = 0;       // GLFW_KEY_*
    int scancode = 0;
    int mods = 0;

    // Named editing operations (resolved from key + mods)
    bool isBackspace() const { return key == 259; }
    bool isDelete()    const { return key == 261; }
    bool isLeft()      const { return key == 263; }
    bool isRight()     const { return key == 262; }
    bool isUp()        const { return key == 265; }
    bool isDown()      const { return key == 264; }
    bool isHome()      const { return key == 268; }
    bool isEnd()       const { return key == 269; }
    bool isEnter()     const { return key == 257 || key == 335; }
    bool isEscape()    const { return key == 256; }
    bool isTab()       const { return key == 258; }
    bool isPageUp()    const { return key == 266; }
    bool isPageDown()  const { return key == 267; }

    // Clipboard operations (Ctrl/Cmd + key)
    bool isCopy()  const { return (mods & 0x2) && key == 67; }  // GLFW_MOD_CONTROL + C
    bool isCut()   const { return (mods & 0x2) && key == 88; }  // GLFW_MOD_CONTROL + X
    bool isPaste() const { return (mods & 0x2) && key == 86; }  // GLFW_MOD_CONTROL + V
    bool isUndo()  const { return (mods & 0x2) && key == 90; }  // GLFW_MOD_CONTROL + Z
    bool isRedo()  const { return (mods & 0x2) && key == 89; }  // GLFW_MOD_CONTROL + Y
    bool isSelectAll() const { return (mods & 0x2) && key == 65; } // GLFW_MOD_CONTROL + A
};

struct CharEvent {
    uint32_t codepoint = 0;

    // Check for special input bytes
    bool isBackspace() const { return codepoint == 8; }
    bool isDelete()    const { return codepoint == 127; }
    bool isEnter()     const { return codepoint == '\r' || codepoint == '\n'; }
    bool isTab()       const { return codepoint == '\t'; }
    bool isPrintable() const { return codepoint >= 32 && codepoint != 127; }
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

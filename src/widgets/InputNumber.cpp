#include "InputNumber.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include <algorithm>
#include <cstdlib>

namespace ui {

InputNumber::InputNumber() { m_focusable = true; }

void InputNumber::setValue(int v) { m_value = v; if (m_onChanged) m_onChanged(m_value); }
void InputNumber::applyValue(int v) {
    int old = m_value;
    m_value = std::clamp(v, m_min, m_max);
    if (m_value != old && m_onChanged) m_onChanged(m_value);
}

void InputNumber::paint(Painter& p) {
    Rect r = screenRect();
    Color bg = m_focused ? Color::fromHex(0xFF2A2A2A) : Color::fromHex(0xFF1E1E1E);
    p.drawRect(r, bg);
    p.drawRectOutline(r, m_focused ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF444444), 1);

    // Up/down buttons on right
    float btnW = 24, midY = r.y + r.height*0.5f;
    p.drawRect({r.x+r.width-btnW, r.y, btnW, r.height}, Color::fromHex(0xFF333344));
    p.drawRect({r.x+r.width-btnW, r.y, btnW, 1}, Color::fromHex(0xFF555566));
    p.drawText({r.x+r.width-btnW, r.y, btnW, midY-r.y}, "^", Color{0.7f,0.7f,0.7f,1}, TextAlign::Center);
    p.drawText({r.x+r.width-btnW, midY, btnW, r.height-(midY-r.y)}, "v", Color{0.7f,0.7f,0.7f,1}, TextAlign::Center);

    // Value text
    std::string txt = std::to_string(m_value);
    p.drawText({r.x+8, r.y+2, r.width-btnW-16, r.height-4}, txt, Color{1,1,1,1}, TextAlign::Left);
}

bool InputNumber::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    float btnW = 24, midY = m_bounds.height*0.5f;
    if (e.localPos.x >= m_bounds.width - btnW) {
        if (e.localPos.y < midY) applyValue(m_value + m_step);
        else applyValue(m_value - m_step);
        return true;
    }
    return true;
}

bool InputNumber::onKeyDown(KeyEvent& e) {
    if (e.type != KeyEvent::Down) return false;
    if (e.key == 265) applyValue(m_value + m_step); // Up
    else if (e.key == 264) applyValue(m_value - m_step); // Down
    else return false;
    return true;
}

bool InputNumber::onCharInput(CharEvent& e) {
    if (e.codepoint >= '0' && e.codepoint <= '9') {
        int d = e.codepoint - '0';
        applyValue(m_value * 10 + d);
        return true;
    }
    if (e.codepoint == 8) { applyValue(m_value / 10); return true; } // backspace
    return false;
}

} // namespace ui

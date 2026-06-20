#include "Button.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

namespace ui {

Button::Button(const std::string& label) : m_label(label) {
    m_focusable = true;
    m_tabIndex = 0;
}

void Button::setLabel(const std::string& text) { m_label = text; }

void Button::setOnClick(std::function<void()> callback) { m_onClick = std::move(callback); }

void Button::click() { if (m_onClick) m_onClick(); }

Size Button::measure(const Size&) const {
    float w = m_label.size() * 8.0f + m_padding.horizontal();
    float h = 18.0f + m_padding.vertical();
    if (m_font) {
        w = m_font->measureText(m_label) + m_padding.horizontal();
        h = m_font->lineHeight() + m_padding.vertical();
    }
    return {w, h};
}

void Button::getCurrentColors(Color& top, Color& bottom) const {
    if (m_state == Pressed && m_usePressColor) {
        top = bottom = m_pressColor; return;
    }
    if (m_state == Hovered && m_useHoverColor) {
        top = bottom = m_hoverColor; return;
    }
    if (m_useGradient) { top = m_gradTop; bottom = m_gradBottom; return; }
    top = bottom = m_solidColor;
}

void Button::paint(Painter& painter) {
    Rect r = screenRect();
    Color top, bottom;
    getCurrentColors(top, bottom);
    top.a *= m_opacity; bottom.a *= m_opacity;

    if (m_cornerRadius > 0) {
        painter.drawRoundedGradientRect(r, top, bottom, m_cornerRadius);
    } else {
        painter.drawGradientRect(r, top, bottom);
    }

    if (m_focusRing) {
        Color fr{1,1,1,m_opacity};
        painter.drawRectOutline(r.insetBy(-2, -2), fr, 2);
    }

    if (!m_label.empty()) {
        painter.pushClip(r);
        Color tc{1, 1, 1, m_opacity};
        painter.drawText(r, m_label, tc, TextAlign::Center);
        painter.popClip();
    }
}

bool Button::onMouseDown(MouseEvent& e) {
    if (e.button == 0) { m_state = Pressed; return true; }
    return false;
}

bool Button::onMouseUp(MouseEvent& e) {
    if (e.button == 0 && m_state == Pressed) { m_state = Hovered; click(); return true; }
    return false;
}

bool Button::onMouseEnter(MouseEvent&) { m_state = Hovered; return true; }
bool Button::onMouseLeave(MouseEvent&) { m_state = Normal; return true; }

bool Button::onKeyDown(KeyEvent& e) {
    if (e.type == KeyEvent::Down && (e.key == 257 || e.key == 32)) { click(); return true; }
    return false;
}

void Button::onFocusGained() { m_focusRing = true; }
void Button::onFocusLost() { m_focusRing = false; }

} // namespace ui

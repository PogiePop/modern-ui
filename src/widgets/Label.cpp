#include "Label.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"

namespace ui {

Label::Label(const std::string& text)
    : m_text(text) {
    m_focusable = false;
}

void Label::setText(const std::string& text) {
    m_text = text;
}

Size Label::measure(const Size&) const {
    float textW = m_text.size() * m_fontSize * 0.55f;
    float textH = m_fontSize * 1.3f;
    if (m_font) {
        textW = m_font->measureText(m_text);
        textH = m_font->lineHeight();
    }
    return {textW, textH};
}

Rect Label::screenRect() const {
    Point g = globalPosition();
    float tw = m_font ? m_font->measureText(m_text) : m_text.size() * m_fontSize * 0.55f;
    // Use max of bounds width and text width to avoid clipping
    float w = m_bounds.width > tw ? m_bounds.width : tw;
    return {g.x, g.y, w, m_bounds.height};
}

void Label::paint(Painter& painter) {
    if (m_text.empty()) return;
    Rect r = screenRect();
    Color c = m_useCustomColor ? m_color : Color{1, 1, 1, 1};
    c.a *= m_opacity;
    painter.drawText(r, m_text, c, m_align);
}

} // namespace ui

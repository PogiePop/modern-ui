#include "Badge.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"

namespace ui {

Badge::Badge(const std::string& text, Variant v) : m_text(text) {
    m_focusable = false;
    if (v != Default) setVariant(v);
}

void Badge::setVariant(Variant v) {
    m_variant = v;
    m_customColor = false;
}

void Badge::setColor(const Color& bg, const Color& text) {
    m_bgColor = bg; m_textColor = text; m_customColor = true;
}

Size Badge::measure(const Size&) const {
    float w = m_text.size() * 8.0f + 16;
    float h = 20.0f;
    return {w, h};
}

void Badge::paint(Painter& p) {
    Rect r = screenRect();

    // Resolve colors from theme or variant
    if (!m_customColor && m_theme) {
        switch (m_variant) {
        case Primary: m_bgColor = m_theme->color(ColorRole::Primary); m_textColor = Color{1,1,1,1}; break;
        case Success: m_bgColor = m_theme->color(ColorRole::Success); m_textColor = Color{1,1,1,1}; break;
        case Danger:  m_bgColor = m_theme->color(ColorRole::Danger);  m_textColor = Color{1,1,1,1}; break;
        case Warning: m_bgColor = m_theme->color(ColorRole::Warning); m_textColor = Color{0,0,0,0.9f}; break;
        case Info:    m_bgColor = m_theme->color(ColorRole::Info);    m_textColor = Color{1,1,1,1}; break;
        default:      m_bgColor = m_theme->color(ColorRole::Surface); m_textColor = m_theme->color(ColorRole::TextSecondary); break;
        }
    }

    float rad = r.height * 0.5f;
    p.drawRoundedRect(r, m_bgColor, rad);
    p.drawText(r, m_text, m_textColor, TextAlign::Center);
}

} // namespace ui

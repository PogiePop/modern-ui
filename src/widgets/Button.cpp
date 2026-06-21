#include "Button.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"

namespace ui {

Button::Button(const std::string& label) : m_label(label) {
    m_focusable = true;
    m_tabIndex = 0;
}

void Button::setLabel(const std::string& text) { m_label = text; }
void Button::setOnClick(std::function<void()> callback) { m_onClick = std::move(callback); }
void Button::click() { if (m_onClick) m_onClick(); }

void Button::setVariant(Variant v) {
    m_variant = v;
    m_manualColor = false;
}

void Button::applyVariantColors() {
    if (m_manualColor || !m_theme) return;
    Color base, hover, press;
    switch (m_variant) {
    case Primary:
        base = m_theme->color(ColorRole::Primary);
        hover = m_theme->color(ColorRole::PrimaryHover);
        press = m_theme->color(ColorRole::PrimaryActive);
        break;
    case Secondary:
        base = m_theme->color(ColorRole::Surface);
        hover = m_theme->color(ColorRole::SurfaceHover);
        press = m_theme->color(ColorRole::SurfaceActive);
        break;
    case Danger:
        base = m_theme->color(ColorRole::Danger);
        hover = m_theme->color(ColorRole::DangerHover);
        press = Color{press.r * 0.8f, press.g * 0.8f, press.b * 0.8f, 1};
        break;
    case Success:
        base = m_theme->color(ColorRole::Success);
        hover = m_theme->color(ColorRole::SuccessHover);
        press = Color{press.r * 0.8f, press.g * 0.8f, press.b * 0.8f, 1};
        break;
    case Ghost:
        base = Color{0, 0, 0, 0};
        hover = m_theme->color(ColorRole::SurfaceHover);
        press = m_theme->color(ColorRole::SurfaceActive);
        break;
    }
    m_solidColor = base;
    m_hoverColor = hover;
    m_pressColor = press;
    m_useGradient = false;
    m_useHoverColor = true;
    m_usePressColor = true;
}

Size Button::measure(const Size&) const {
    float w = m_label.size() * 8.0f + m_padding.horizontal();
    float h = 18.0f + m_padding.vertical();
    if (m_font) {
        w = m_font->measureText(m_label) + m_padding.horizontal();
        h = m_font->lineHeight() + m_padding.vertical();
    }
    if (w < m_minWidth) w = m_minWidth;
    return {w, h};
}

void Button::getCurrentColors(Color& top, Color& bottom) {
    applyVariantColors();
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

    // Shadow (for non-ghost buttons)
    if (m_variant != Ghost && m_variant != Secondary) {
        Color sc{0, 0, 0, 0.15f * m_opacity};
        if (m_state == Hovered) sc.a *= 1.5f;
        painter.drawShadow(r, m_cornerRadius, sc, {0, 2}, 6);
    }

    // Background
    float cr = m_cornerRadius > 0 ? m_cornerRadius : 8;
    if (m_useGradient && top.r != bottom.r) {
        painter.drawRoundedGradientRect(r, top, bottom, cr);
    } else {
        painter.drawRoundedRect(r, top, cr);
    }

    // Border for ghost/secondary variants
    if (m_variant == Ghost || m_variant == Secondary) {
        Color borderCol = m_theme ? m_theme->color(ColorRole::Border) : Color{0.3f, 0.3f, 0.4f, 1};
        painter.drawRectOutline(r, borderCol, 1.0f);
    }

    // Focus ring
    if (m_focusRing) {
        Color fr = m_theme ? m_theme->color(ColorRole::BorderFocused) : Color{1, 1, 1, 0.7f};
        painter.drawRectOutline(r.insetBy(-2, -2), fr, 2);
    }

    // Icon (left of label)
    Color tc{1, 1, 1, m_opacity};
    if (m_variant == Ghost || m_variant == Secondary)
        tc = m_theme ? m_theme->color(ColorRole::Text) : Color{1, 1, 1, 1};

    if (!m_label.empty()) {
        float iconSize = m_font ? m_font->lineHeight() : 16;
        float totalW = 0;
        if (m_icon != IconType::None) totalW += iconSize + 4;
        totalW += m_font ? m_font->measureText(m_label) : m_label.size() * 8.0f;
        float startX = r.x + (r.width - totalW) * 0.5f;
        if (m_icon != IconType::None) {
            drawIcon(painter, m_icon, startX, r.y + (r.height - iconSize) * 0.5f, iconSize, tc);
            startX += iconSize + 4;
        }
        painter.drawText({startX, r.y, r.width - (startX - r.x), r.height}, m_label, tc, TextAlign::Left);
    } else if (m_icon != IconType::None) {
        float iconSize = m_font ? m_font->lineHeight() : 18;
        drawIcon(painter, m_icon, r.x + (r.width - iconSize) * 0.5f, r.y + (r.height - iconSize) * 0.5f, iconSize, tc);
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

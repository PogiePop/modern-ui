#include "Space.hpp"
#include "core/Painter.hpp"

namespace ui {

Spacer::Spacer(SpaceSize min) : m_min((float)min) { m_focusable = false; }

Divider::Divider(Direction dir, DividerStyle style, float thickness, const Color& color)
    : m_dir(dir), m_style(style), m_thickness(thickness), m_color(color) { m_focusable = false; }

void Divider::paint(Painter& p) {
    Rect r = screenRect();
    if (m_dir == Horizontal) {
        float step = m_style == DividerStyle::Dotted ? m_thickness * 3 : m_thickness * 2;
        for (float x = r.x; x < r.x + r.width; x += step) {
            float w = m_style == DividerStyle::Solid ? r.width : m_thickness * 1.5f;
            if (w > r.x + r.width - x) w = r.x + r.width - x;
            p.drawRect({x, r.y + (r.height - m_thickness)*0.5f, w, m_thickness}, m_color);
        }
    } else {
        float step = m_style == DividerStyle::Dotted ? m_thickness * 3 : m_thickness * 2;
        for (float y = r.y; y < r.y + r.height; y += step) {
            float h = m_style == DividerStyle::Solid ? r.height : m_thickness * 1.5f;
            if (h > r.y + r.height - y) h = r.y + r.height - y;
            p.drawRect({r.x + (r.width - m_thickness)*0.5f, y, m_thickness, h}, m_color);
        }
    }
}

} // namespace ui

#include "Border.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"

namespace ui {
Border::Border() { m_focusable = false; }

void Border::setChild(std::unique_ptr<Widget> child) {
    removeAllChildren(); if (child) addChild(std::move(child));
}
void Border::setStroke(float t, const Color& c) { m_thickness = t; m_color = c; m_manualColor = true; }

Size Border::measure(const Size& a) const {
    float t2 = m_thickness * 2;
    if (m_children.empty()) return {t2, t2};
    Size cs = m_children[0]->measure({a.width-t2, a.height-t2});
    return {cs.width+t2, cs.height+t2};
}
void Border::layout() {
    if (m_children.empty()) return;
    float t = m_thickness;
    m_children[0]->setBounds({t, t, m_bounds.width-t*2, m_bounds.height-t*2});
    m_children[0]->layout();
}
void Border::paint(Painter& p) {
    Rect r = screenRect();
    Color c = m_color;
    if (m_theme && m_useColorRole) c = m_theme->color(m_colorRole);
    else if (m_theme && !m_manualColor) c = m_theme->color(ColorRole::Border);

    float rad = m_radius;
    float step = m_borderStyle == Dotted ? m_thickness * 3 : (m_borderStyle == Dashed ? m_thickness * 4 : m_thickness * 0.5f);

    if (m_borderStyle == Solid) {
        for (float i = 0; i < m_thickness; i += 1) {
            if (rad > 0) p.drawRoundedRect(r, Color{0,0,0,0}, rad);
            p.drawRectOutline(r.insetBy(-i, -i), c, 1);
        }
    } else {
        // Dashed / Dotted: draw segments along edges
        float t = m_thickness;
        for (float i = 0; i < t; i += 1) {
            float off = -i;
            // Top
            for (float x = r.x; x < r.x + r.width; x += step)
                p.drawRect({x, r.y+off, std::min(step*0.6f, r.x+r.width-x), t}, c);
            // Bottom
            for (float x = r.x; x < r.x + r.width; x += step)
                p.drawRect({x, r.y+r.height-t-off, std::min(step*0.6f, r.x+r.width-x), t}, c);
            // Left
            for (float y = r.y; y < r.y + r.height; y += step)
                p.drawRect({r.x+off, y, t, std::min(step*0.6f, r.y+r.height-y)}, c);
            // Right
            for (float y = r.y; y < r.y + r.height; y += step)
                p.drawRect({r.x+r.width-t-off, y, t, std::min(step*0.6f, r.y+r.height-y)}, c);
        }
    }
    if (!m_children.empty()) m_children[0]->paint(p);
}
} // namespace ui

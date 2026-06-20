#include "Border.hpp"
#include "core/Painter.hpp"

namespace ui {
Border::Border() { m_focusable = false; }

void Border::setChild(std::unique_ptr<Widget> child) {
    removeAllChildren(); if (child) addChild(std::move(child));
}
void Border::setStroke(float t, const Color& c) { m_thickness = t; m_color = c; }

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
    for (float i = 0; i < m_thickness; i += 1)
        p.drawRectOutline(r.insetBy(-i, -i), m_color, 1);
    if (!m_children.empty()) m_children[0]->paint(p);
}
} // namespace ui

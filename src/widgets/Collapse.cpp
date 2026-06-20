#include "Collapse.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"

namespace ui {
CollapsePanel::CollapsePanel(const std::string& title) : m_title(title) { m_focusable = false; }

void CollapsePanel::setContent(std::unique_ptr<Widget> c) {
    removeAllChildren(); if (c) { c->setVisible(m_expanded); addChild(std::move(c)); }
}
void CollapsePanel::setExpanded(bool e) {
    if (m_expanded == e) return;
    m_expanded = e;
    for (auto& ch : m_children) ch->setVisible(e);
    // Trigger relayout so parent re-measures and pushes content down
    Widget* w = this;
    while (w->parent()) w = w->parent();
    w->markLayoutDirty();
}

Size CollapsePanel::measure(const Size& a) const {
    float h = HEADER_H;
    if (m_expanded && !m_children.empty()) h += m_children[0]->measure({a.width, a.height-HEADER_H}).height;
    return {a.width, h};
}

void CollapsePanel::layout() {
    if (m_expanded && !m_children.empty()) {
        m_children[0]->setBounds({0, HEADER_H, m_bounds.width, m_bounds.height-HEADER_H});
        m_children[0]->layout();
    }
}

void CollapsePanel::paint(Painter& p) {
    Rect r = screenRect();
    // Header
    Color hdrBg = m_expanded ? Color::fromHex(0xFF2A2A3E) : Color::fromHex(0xFF1E1E2E);
    p.drawRect({r.x, r.y, m_bounds.width, HEADER_H}, hdrBg);
    // Arrow indicator
    float ax = r.x + 12, ay = r.y + HEADER_H*0.5f;
    p.drawText({r.x+28, r.y, m_bounds.width-40, HEADER_H}, m_title, Color{1,1,1,1}, TextAlign::Left);
    p.drawText({ax, r.y, 16, HEADER_H}, m_expanded ? "v" : ">", Color{0.6f,0.6f,0.6f,1}, TextAlign::Center);
    // Content
    if (m_expanded && !m_children.empty()) m_children[0]->paint(p);
}

bool CollapsePanel::onMouseDown(MouseEvent& e) {
    if (e.button == 0 && e.localPos.y < HEADER_H) { setExpanded(!m_expanded); return true; }
    return false;
}
} // namespace ui

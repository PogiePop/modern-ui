#include "Collapse.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"

namespace ui {
CollapsePanel::CollapsePanel(const std::string& title) : m_title(title) { m_focusable = false; }

void CollapsePanel::setContent(std::unique_ptr<Widget> c) {
    removeAllChildren(); if (c) { c->setVisible(m_expanded); addChild(std::move(c)); }
}
void CollapsePanel::setExpanded(bool e) {
    if (m_expanded == e) return;
    m_expanded = e;
    for (auto& ch : m_children) ch->setVisible(e);
    Widget* w = this;
    while (w->parent()) w = w->parent();
    w->markLayoutDirty();
}
Size CollapsePanel::measure(const Size& a) const {
    float h = m_headerH;
    if (m_expanded && !m_children.empty()) h += m_children[0]->measure({a.width, a.height-m_headerH}).height;
    return {a.width, h};
}
void CollapsePanel::layout() {
    if (m_expanded && !m_children.empty()) {
        m_children[0]->setBounds({0, m_headerH, m_bounds.width, m_bounds.height-m_headerH});
        m_children[0]->layout();
    }
}
void CollapsePanel::paint(Painter& p) {
    Rect r = screenRect();

    // Resolve colors from theme
    Color hdrBg = m_expanded ? Color::fromHex(0xFF2A2A3E) : Color::fromHex(0xFF1E1E2E);
    Color textCol = Color{1,1,1,1};
    Color arrowCol = Color{0.6f,0.6f,0.6f,1};
    Color accentCol = m_expanded ? Color::fromHex(0xFF3B82F6) : arrowCol;
    Color borderCol = Color::fromHex(0xFF2E2E3E);
    Color bgCol = Color::fromHex(0xFF1A1A2E);

    if (m_theme) {
        hdrBg = m_expanded ? m_theme->color(ColorRole::SurfaceActive) : m_theme->color(ColorRole::Surface);
        textCol = m_theme->color(ColorRole::Text);
        arrowCol = m_theme->color(ColorRole::TextSecondary);
        accentCol = m_useColorRole ? m_theme->color(m_colorRole) : m_theme->color(ColorRole::Primary);
        borderCol = m_theme->color(ColorRole::Border);
        bgCol = m_theme->color(ColorRole::Bg);
    }

    // Background for Card variant
    if (m_variant == Card) {
        p.drawRoundedRect(r, bgCol, 8);
        p.drawRectOutline(r, borderCol, 1);
    }

    // Header background
    float rad = m_variant == Bordered ? 0 : 4;
    p.drawRoundedRect({r.x + 2, r.y + 2, m_bounds.width - 4, m_headerH - 2}, hdrBg, rad);

    // Left accent bar
    if (m_variant != Default) {
        p.drawRect({r.x + 2, r.y + 4, 3, m_headerH - 8}, accentCol);
    }

    // Arrow
    float ax = r.x + (m_variant != Default ? 16 : 12);
    p.drawText({ax, r.y, 16, m_headerH}, m_expanded ? "v" : ">",
               m_expanded ? accentCol : arrowCol, TextAlign::Center);

    // Icon (if set)
    float tx = ax + 16;
    if (m_icon != IconType::None) {
        float iconSize = m_headerH - 8;
        drawIcon(p, m_icon, tx, r.y + 4, iconSize, m_expanded ? accentCol : arrowCol);
        tx += iconSize + 4;
    }

    // Title
    p.drawText({tx, r.y, m_bounds.width - tx - 8, m_headerH}, m_title, textCol, TextAlign::Left);

    // Content padding area for Card variant
    if (m_variant == Card && m_expanded && !m_children.empty()) {
        p.drawRect({r.x, r.y + m_headerH, m_bounds.width, 4}, bgCol);
    }

    // Content
    if (m_expanded && !m_children.empty()) m_children[0]->paint(p);
}
bool CollapsePanel::onMouseDown(MouseEvent& e) {
    if (e.button == 0 && e.localPos.y < m_headerH) { setExpanded(!m_expanded); return true; }
    return false;
}
} // namespace ui

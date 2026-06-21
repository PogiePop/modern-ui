#include "Card.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"

namespace ui {

Card::Card() : Container(LayoutDirection::Vertical) {
    setPadding(EdgeInsets::all(16));
    setSpacing(8);
}

void Card::paint(Painter& painter) {
    Rect r = screenRect();
    float rad = m_useRadius ? m_radius : 8.0f;

    // Background
    Color bg = Color::fromHex(0xFF1E1E2E); // default dark surface
    if (m_theme) {
        bg = m_theme->color(ColorRole::Surface);
    }
    if (hasBackgroundColor()) bg = backgroundColor();
    painter.drawRoundedRect(r, bg, rad);

    // Shadow (if elevated)
    if (m_elevation > 0) {
        float alpha = m_elevation * 0.04f;
        if (alpha > 0.3f) alpha = 0.3f;
        painter.drawShadow(r, rad, Color{0, 0, 0, alpha}, {0, m_elevation * 0.5f}, m_elevation * 1.5f);
    }

    // Border
    Color borderCol = Color::fromHex(0xFF2E2E3E);
    if (m_theme) borderCol = m_theme->color(ColorRole::Border);
    painter.drawRectOutline(r, borderCol, 1.0f);

    // Children
    for (auto& child : m_children) {
        if (!child->visible()) continue;
        child->paint(painter);
    }
}

} // namespace ui

#include "Tooltip.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"
#include "Widget.hpp"

namespace ui {

TooltipManager& TooltipManager::instance() {
    static TooltipManager mgr;
    return mgr;
}

void TooltipManager::setTooltip(Widget* target, const std::string& text) {
    m_target = target; m_text = text;
}

void TooltipManager::clearTooltip(Widget* target) {
    if (m_target == target) { m_target = nullptr; m_text.clear(); m_visible = false; }
}

void TooltipManager::showFor(Widget* target, float x, float y) {
    if (m_target == target && !m_text.empty()) { m_visible = true; m_x = x; m_y = y; }
}
void TooltipManager::showAt(float x, float y) {
    if (!m_text.empty()) { m_visible = true; m_x = x; m_y = y; }
}

void TooltipManager::hide() { m_visible = false; }

void TooltipManager::render(Painter& painter, Font* font) {
    if (!m_visible || m_text.empty()) return;
    float pad = 8, fh = font ? font->lineHeight() : 16;
    float tw = font ? font->measureText(m_text) : m_text.size() * 8.0f;
    if (tw > m_maxWidth) tw = m_maxWidth;

    float bw = tw + pad * 2, bh = fh + pad * 2;

    // Always show below and centered on the anchor point (mouse position)
    float tx = m_x - bw * 0.5f;
    float ty = m_y + 18;

    // Apply position offset
    if (m_position == Top)    ty = m_y - bh - 8;
    if (m_position == Bottom) ty = m_y + 18;
    if (m_position == Left)   { tx = m_x - bw - 10; ty = m_y - bh * 0.5f; }
    if (m_position == Right)  { tx = m_x + 10;        ty = m_y - bh * 0.5f; }

    // Clamp to screen (avoid clipping)
    if (tx < 4) tx = 4;
    if (ty < 4) ty = 4;

    Rect r{tx, ty, bw, bh};
    painter.drawRoundedRect(r, Color::fromHex(0xDD222233), 5);
    painter.drawRectOutline(r, Color::fromHex(0x88444466), 1);
    painter.drawText(r.insetBy(pad, pad), m_text, {1, 1, 1, 1}, TextAlign::Left);
}

void setTooltip(Widget* w, const std::string& text) {
    TooltipManager::instance().setTooltip(w, text);
}

} // namespace ui

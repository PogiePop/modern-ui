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
    // Background bubble
    float pad = 6, fh = font ? font->lineHeight() : 16;
    float tw = font ? font->measureText(m_text) : m_text.size() * 8.0f;
    Rect r{m_x + 10, m_y + 14, tw + pad*2, fh + pad*2};
    painter.drawRoundedRect(r, Color::fromHex(0xDD222233), 4);
    painter.drawRectOutline(r, Color::fromHex(0x88444466), 1);
    painter.drawText(r.insetBy(pad, pad), m_text, {1,1,1,1}, TextAlign::Left);
}

void setTooltip(Widget* w, const std::string& text) {
    TooltipManager::instance().setTooltip(w, text);
}

} // namespace ui

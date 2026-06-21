#include "Checkbox.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"

namespace ui {

Checkbox::Checkbox(const std::string& label) : m_label(label) { m_focusable = true; m_tabIndex = 0; }
void Checkbox::setChecked(bool checked) { m_checked = checked; }
void Checkbox::setLabel(const std::string& label) { m_label = label; }
void Checkbox::setOnCheckedChanged(std::function<void(bool)> callback) { m_onCheckedChanged = std::move(callback); }

Size Checkbox::measure(const Size&) const {
    float textW = m_label.size() * 8.0f;
    float textH = m_boxSize;
    if (m_font) { textW = m_font->measureText(m_label); textH = m_font->lineHeight(); }
    if (textH < m_boxSize) textH = m_boxSize;
    return {m_boxSize + m_gap + textW, textH};
}

void Checkbox::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    float cy = sy + m_bounds.height * 0.5f;
    float bs = m_boxSize;
    Rect boxRect{sx, cy - bs * 0.5f, bs, bs};

    Color boxFill = m_checked ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF333333);
    Color borderCol = Color::fromHex(0xFF555555);
    Color checkCol{1, 1, 1, 1};
    Color textCol = Color::fromHex(0xFFCCCCCC);

    if (m_theme) {
        Color roleCol = m_theme->color(m_useColorRole ? m_colorRole : ColorRole::Primary);
        boxFill = m_checked ? roleCol : m_theme->color(ColorRole::Surface);
        borderCol = m_focusRing ? m_theme->color(ColorRole::BorderFocused) : m_theme->color(ColorRole::Border);
        textCol = m_hovered ? m_theme->color(ColorRole::Text) : m_theme->color(ColorRole::TextSecondary);
    }

    float rad = bs * 0.22f;
    painter.drawRoundedRect(boxRect, boxFill, rad);
    painter.drawRectOutline(boxRect, borderCol, 1.5f);

    if (m_checked) {
        float cx2 = sx + bs * 0.5f;
        float w1 = bs * 0.11f, w2 = bs * 0.33f, w3 = bs * 0.17f;
        painter.drawRect({cx2 - w2, cy - w1, w3, w1 * 2}, checkCol);
        painter.drawRect({cx2 - w1, cy + w1, w2 * 2, w1 * 2}, checkCol);
    }

    if (!m_label.empty()) {
        Rect textBounds{sx + bs + m_gap, sy, m_bounds.width - bs - m_gap, m_bounds.height};
        painter.drawText(textBounds, m_label, textCol, TextAlign::Left);
    }
}
bool Checkbox::onMouseDown(MouseEvent& e) { if (e.button == 0) { m_checked = !m_checked; if (m_onCheckedChanged) m_onCheckedChanged(m_checked); return true; } return false; }
bool Checkbox::onKeyDown(KeyEvent& e) { if (e.type == KeyEvent::Down && (e.key == 32 || e.key == 257)) { m_checked = !m_checked; if (m_onCheckedChanged) m_onCheckedChanged(m_checked); return true; } return false; }
bool Checkbox::onMouseEnter(MouseEvent&) { m_hovered = true; return true; }
bool Checkbox::onMouseLeave(MouseEvent&) { m_hovered = false; return true; }
void Checkbox::onFocusGained() { m_focusRing = true; }
void Checkbox::onFocusLost() { m_focusRing = false; }

} // namespace ui

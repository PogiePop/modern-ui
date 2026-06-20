#include "Checkbox.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

namespace ui {

Checkbox::Checkbox(const std::string& label)
    : m_label(label) {
    m_focusable = true;
    m_tabIndex = 0;
}

void Checkbox::setChecked(bool checked) {
    m_checked = checked;
}

void Checkbox::setLabel(const std::string& label) {
    m_label = label;
}

void Checkbox::setOnCheckedChanged(std::function<void(bool)> callback) {
    m_onCheckedChanged = std::move(callback);
}

Size Checkbox::measure(const Size&) const {
    float textW = m_label.size() * 8.0f;
    float textH = BOX_SIZE;
    if (m_font) { textW = m_font->measureText(m_label); textH = m_font->lineHeight(); }
    if (textH < BOX_SIZE) textH = BOX_SIZE;
    return {BOX_SIZE + GAP + textW, textH};
}

void Checkbox::paint(Painter& painter) {
    float sx = screenRect().x;
    float sy = screenRect().y;
    // Box bounds
    Rect boxRect{sx, sy, BOX_SIZE, BOX_SIZE};

    // Box fill
    Color boxFill = m_checked
        ? Color::fromHex(0xFF3B82F6)
        : Color::fromHex(0xFF333333);
    painter.drawRect(boxRect, boxFill);

    // Box border
    Color borderColor = m_focusRing
        ? Color::fromHex(0xFFFFFFFF)
        : Color::fromHex(0xFF555555);
    painter.drawRectOutline(boxRect, borderColor, 1);

    // Checkmark (simple cross pattern when checked)
    if (m_checked) {
        // Draw a simple check: two diagonal lines approximating a checkmark
        float cx = sx + 9, cy = sy + 9;
        Color white{1, 1, 1, 1};
        painter.drawRect({cx - 4, cy - 1, 3, 2}, white);
        painter.drawRect({cx - 1, cy - 3, 2, 7}, white);
    }

    // Label text
    if (!m_label.empty()) {
        Rect r = screenRect();
        painter.pushClip(r);
        Rect textBounds{sx + BOX_SIZE + GAP, sy, m_bounds.width - BOX_SIZE - GAP, m_bounds.height};
        Color textColor = m_hovered ? Color::fromHex(0xFFFFFFFF) : Color::fromHex(0xFFCCCCCC);
        painter.drawText(textBounds, m_label, textColor, TextAlign::Left);
        painter.popClip();
    }
}

bool Checkbox::onMouseDown(MouseEvent& event) {
    if (event.button == 0) {
        m_checked = !m_checked;
        if (m_onCheckedChanged) m_onCheckedChanged(m_checked);
        return true;
    }
    return false;
}

bool Checkbox::onKeyDown(KeyEvent& event) {
    if (event.type == KeyEvent::Down) {
        // Space or Enter to toggle
        // GLFW_KEY_SPACE = 32
        if (event.key == 32 || event.key == 257) { // 257 = Enter on some layouts
            m_checked = !m_checked;
            if (m_onCheckedChanged) m_onCheckedChanged(m_checked);
            return true;
        }
    }
    return false;
}

bool Checkbox::onMouseEnter(MouseEvent&) {
    m_hovered = true;
    return true;
}

bool Checkbox::onMouseLeave(MouseEvent&) {
    m_hovered = false;
    return true;
}

void Checkbox::onFocusGained() {
    m_focusRing = true;
}

void Checkbox::onFocusLost() {
    m_focusRing = false;
}

} // namespace ui

#include "RadioButton.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

namespace ui {

void RadioGroup::add(RadioButton* btn) {
    if (!btn) return;
    m_buttons.push_back(btn);
    btn->m_group = this;
}

void RadioGroup::remove(RadioButton* btn) {
    auto it = std::find(m_buttons.begin(), m_buttons.end(), btn);
    if (it != m_buttons.end()) {
        if (m_selected == *it) m_selected = nullptr;
        (*it)->m_group = nullptr;
        m_buttons.erase(it);
    }
}

void RadioGroup::select(RadioButton* btn) {
    if (m_selected == btn) return;
    if (m_selected) m_selected->setSelected(false);
    m_selected = btn;
    if (m_selected) m_selected->setSelected(true);
}

RadioButton::RadioButton(const std::string& label, RadioGroup* group)
    : m_label(label), m_group(group) {
    m_focusable = true;
    m_tabIndex = 0;
    if (m_group) m_group->add(this);
}

void RadioButton::setLabel(const std::string& label) { m_label = label; }

void RadioButton::setSelected(bool sel) {
    m_selected = sel;
    if (m_group && sel && m_group->selected() != this) {
        m_group->select(this);
    }
    if (m_onSelectedChanged) m_onSelectedChanged(m_selected);
}

void RadioButton::setOnSelectedChanged(std::function<void(bool)> cb) {
    m_onSelectedChanged = std::move(cb);
}

void RadioButton::setGroup(RadioGroup* group) {
    if (m_group) m_group->remove(this);
    m_group = group;
    if (m_group) m_group->add(this);
}

Size RadioButton::measure(const Size&) const {
    float r = 10;
    float textW = m_label.size() * 8.0f;
    float textH = r * 2 + 4;
    if (m_font) {
        textW = m_font->measureText(m_label);
        textH = m_font->lineHeight();
        if (textH < r * 2) textH = r * 2;
    }
    return {r * 2 + 8 + textW, textH};
}

void RadioButton::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    float r = 10;
    float cx = sx + r, cy = sy + m_bounds.height * 0.5f;
    float outerR = r, innerR = r - 4;

    // Outer circle (filled background + border)
    Color outerFill = m_hovered ? Color::fromHex(0xFF3A3A3A) : Color::fromHex(0xFF2A2A2A);
    painter.drawRoundedRect({cx - outerR, cy - outerR, outerR * 2, outerR * 2}, outerFill, outerR);

    Color borderCol = m_focusRing ? Color::fromHex(0xFFFFFFFF) : Color::fromHex(0xFF555555);
    painter.drawRectOutline({cx - outerR, cy - outerR, outerR * 2, outerR * 2}, borderCol, 1);

    // Inner dot when selected
    if (m_selected) {
        Color dotCol = Color::fromHex(0xFF3B82F6);
        painter.drawRoundedRect({cx - innerR, cy - innerR, innerR * 2, innerR * 2}, dotCol, innerR);
    }

    // Label
    if (!m_label.empty()) {
        Rect textBounds{cx + outerR + 8, sy, m_bounds.width - cx + sx - outerR - 8, m_bounds.height};
        painter.drawText(textBounds, m_label, Color::fromHex(0xFFCCCCCC), TextAlign::Left);
    }
}

bool RadioButton::onMouseDown(MouseEvent& e) {
    if (e.button == 0) { setSelected(true); return true; }
    return false;
}

bool RadioButton::onKeyDown(KeyEvent& e) {
    if (e.type == KeyEvent::Down && (e.key == 32 || e.key == 257)) {
        setSelected(true); return true;
    }
    return false;
}

bool RadioButton::onMouseEnter(MouseEvent&) { m_hovered = true; return true; }
bool RadioButton::onMouseLeave(MouseEvent&) { m_hovered = false; return true; }
void RadioButton::onFocusGained() { m_focusRing = true; }
void RadioButton::onFocusLost() { m_focusRing = false; }

} // namespace ui

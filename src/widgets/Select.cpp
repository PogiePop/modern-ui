#include "Select.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

namespace ui {

Select::Select() { m_focusable = true; }

void Select::addItem(const std::string& item) { m_items.push_back(item); }
void Select::removeItem(size_t i) { if (i < m_items.size()) m_items.erase(m_items.begin()+i); if (m_selected >= m_items.size()) m_selected = m_items.empty()?0:m_items.size()-1; }
void Select::clearItems() { m_items.clear(); m_selected = 0; }

void Select::setSelectedIndex(size_t i) {
    if (i < m_items.size()) { m_selected = i; if (m_onChanged) m_onChanged(i, m_items[i]); }
}
const std::string& Select::selectedText() const {
    static const std::string kEmpty;
    return m_selected < m_items.size() ? m_items[m_selected] : kEmpty;
}

Size Select::measure(const Size&) const { return {150, m_items.size() * ROW_H + 4}; }

void Select::paint(Painter& p) {
    Rect r = screenRect();
    p.drawRoundedRect(r, Color::fromHex(0xFF1E1E2E), 6);
    p.drawRectOutline(r, Color::fromHex(0xFF444466), 1);
    float lh = m_font ? m_font->lineHeight() : 18;
    for (size_t i = 0; i < m_items.size(); ++i) {
        float ry = r.y + 2 + i * ROW_H;
        Color bg{0,0,0,0};
        if ((int)i == m_selected) bg = Color{0.2f,0.4f,0.8f,0.5f};
        else if ((int)i == m_hovered) bg = Color{0.3f,0.3f,0.4f,0.4f};
        if (bg.a > 0) p.drawRect({r.x+2, ry, r.width-4, ROW_H}, bg);
        Color tc = (int)i == m_selected ? Color{1,1,1,1} : Color{0.8f,0.8f,0.8f,1};
        p.drawText({r.x+8, ry+2, r.width-16, lh}, m_items[i], tc, TextAlign::Left);
    }
}

// EUI-NEO: select item by local position
bool Select::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    // e.localPos is relative to Select's top-left (App uses screenRect for conversion)
    int idx = (int)((e.localPos.y - 2) / ROW_H); // -2 for padding
    if (idx >= 0 && idx < (int)m_items.size()) { m_selected = idx; if (m_onChanged) m_onChanged(idx, m_items[idx]); }
    return true;
}
bool Select::onMouseMove(MouseEvent& e) { m_hovered = (int)((e.localPos.y - 2) / ROW_H); return true; }

} // namespace ui

#include "Label.hpp"
#include "ContextMenu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
#include "utils/Clipboard.hpp"
#include <algorithm>

namespace ui {

Label::Label(const std::string& text) : m_text(text) { m_focusable = false; }
Label::~Label() = default;

void Label::setText(const std::string& text) { m_text = text; clearSelection(); }

Size Label::measure(const Size&) const {
    float textW = m_text.size() * m_fontSize * 0.55f;
    float textH = m_fontSize * 1.3f;
    if (m_font) { textW = m_font->measureText(m_text); textH = m_font->lineHeight(); }
    return {textW, textH};
}

Rect Label::screenRect() const {
    Point g = globalPosition();
    float tw = m_font ? m_font->measureText(m_text) : m_text.size() * m_fontSize * 0.55f;
    float w = m_bounds.width > tw ? m_bounds.width : tw;
    return {g.x, g.y, w, m_bounds.height};
}

// ── Selection ──
bool Label::hasSelection() const { return m_selStart != m_selEnd; }
std::string Label::selectedText() const {
    if (!hasSelection()) return "";
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    if (s > m_text.size()) s = m_text.size();
    if (e > m_text.size()) e = m_text.size();
    return m_text.substr(s, e - s);
}
void Label::selectAll() { m_selStart = 0; m_selEnd = m_text.size(); }
void Label::clearSelection() { m_selStart = m_selEnd = 0; }
void Label::copy() { if (hasSelection()) Clipboard::set(selectedText()); }

size_t Label::posAtX(float x) const {
    if (!m_font) return static_cast<size_t>(std::max(0.0f, x / (m_fontSize * 0.55f)));
    size_t lo = 0, hi = m_text.size();
    while (lo < hi) {
        size_t mid = (lo + hi + 1) / 2;
        if (m_font->measureText(m_text.substr(0, mid)) <= x) lo = mid;
        else hi = mid - 1;
    }
    return lo;
}

void Label::showContextMenu(float x, float y) {
    if (!m_contextMenu) m_contextMenu = std::make_unique<ContextMenu>();
    m_contextMenu->clearItems();
    m_contextMenu->addItem("Copy", [this]() { copy(); }, "Ctrl+C", hasSelection());
    m_contextMenu->addItem("Select All", [this]() { selectAll(); }, "Ctrl+A", !m_text.empty());
    m_contextMenu->setFont(m_font);
    if (m_theme) m_contextMenu->setTheme(m_theme);
    m_contextMenu->show(x, y, this);
}

// ── Paint ──
void Label::paint(Painter& painter) {
    if (m_text.empty()) return;
    Rect r = screenRect();

    Color c;
    if (m_useCustomColor) c = m_color;
    else if (m_useColorRole && m_theme) c = m_theme->color(m_colorRole);
    else if (m_theme) c = m_theme->color(ColorRole::Text);
    else c = Color{1, 1, 1, 1};
    c.a *= m_opacity;

    // Selection highlight
    size_t ss = std::min(m_selStart, m_selEnd);
    size_t ee = std::max(m_selStart, m_selEnd);
    if (ss != ee && m_selStart != m_selEnd) {
        // Measure selection position
        float totalW = m_font ? m_font->measureText(m_text) : m_text.size() * m_fontSize * 0.55f;
        float startX = r.x;
        if (m_align == TextAlign::Center) startX = r.x + (r.width - totalW) * 0.5f;
        else if (m_align == TextAlign::Right) startX = r.x + r.width - totalW;

        float sx = startX + (m_font ? m_font->measureText(m_text.substr(0, ss)) : ss * m_fontSize * 0.55f);
        float sw = m_font ? m_font->measureText(m_text.substr(ss, ee - ss)) : (ee - ss) * m_fontSize * 0.55f;

        Color selBg = m_theme ? m_theme->color(ColorRole::Primary) : Color{0.3f, 0.5f, 1, 1};
        selBg.a = 0.3f;
        painter.drawRect({sx, r.y, sw, r.height}, selBg);
    }

    painter.drawText(r, m_text, c, m_align);
}

// ── Mouse ──
bool Label::onMouseDown(MouseEvent& e) {
    if (e.button == 1) { showContextMenu(e.globalPos.x, e.globalPos.y); return true; }
    if (e.button == 0 && m_font) {
        // Map click to character position
        float totalW = m_font->measureText(m_text);
        float startX = screenRect().x;
        if (m_align == TextAlign::Center) startX = screenRect().x + (m_bounds.width - totalW) * 0.5f;
        else if (m_align == TextAlign::Right) startX = screenRect().x + m_bounds.width - totalW;
        float relX = e.globalPos.x - startX;
        size_t pos = posAtX(relX);
        m_selStart = m_selEnd = pos;
        m_dragging = true;
        return true;
    }
    return false;
}
bool Label::onMouseUp(MouseEvent& e) {
    if (e.button == 0) { m_dragging = false; return true; }
    return false;
}
bool Label::onMouseMove(MouseEvent& e) {
    if (m_dragging && m_font) {
        float totalW = m_font->measureText(m_text);
        float startX = screenRect().x;
        if (m_align == TextAlign::Center) startX = screenRect().x + (m_bounds.width - totalW) * 0.5f;
        else if (m_align == TextAlign::Right) startX = screenRect().x + m_bounds.width - totalW;
        float relX = e.globalPos.x - startX;
        m_selEnd = posAtX(relX);
        return true;
    }
    return false;
}
bool Label::onKeyDown(KeyEvent& e) {
    if ((e.mods & 0x2) && e.isCopy()) { copy(); return true; }
    if ((e.mods & 0x2) && e.isSelectAll()) { selectAll(); return true; }
    return false;
}

} // namespace ui

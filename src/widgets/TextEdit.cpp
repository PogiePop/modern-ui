#include "TextEdit.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

#include <algorithm>
#include <cctype>

namespace ui {

// --- Helpers for cursor ↔ (line, col) mapping ---
// Only +1 for actual '\n', not for soft wraps

static void cursorToLineCol(const std::vector<std::string>& lines, const std::string& text,
                             size_t cursorPos, size_t& outLine, size_t& outCol) {
    size_t pos = 0;
    for (size_t i = 0; i < lines.size(); ++i) {
        size_t lineEnd = pos + lines[i].size();
        if (cursorPos <= lineEnd) { outLine = i; outCol = cursorPos - pos; return; }
        pos = lineEnd;
        if (pos < text.size() && text[pos] == '\n') pos += 1;
    }
    outLine = lines.empty() ? 0 : lines.size() - 1;
    outCol = 0;
}

static size_t lineColToCursor(const std::vector<std::string>& lines, const std::string& text,
                               size_t line, size_t col) {
    size_t pos = 0;
    for (size_t i = 0; i < line && i < lines.size(); ++i) {
        pos += lines[i].size();
        if (pos < text.size() && text[pos] == '\n') pos += 1;
    }
    if (line < lines.size()) pos += std::min(col, lines[line].size());
    return std::min(pos, text.size());
}

// --- TextEdit ---

TextEdit::TextEdit(const std::string& placeholder) : m_placeholder(placeholder) {
    m_focusable = true; m_tabIndex = 0;
}

void TextEdit::setText(const std::string& text) { m_text = text; m_cursorPos = m_text.size(); }
void TextEdit::setOnTextChanged(std::function<void(const std::string&)> cb) { m_onTextChanged = std::move(cb); }
float TextEdit::charWidth() const { return m_font ? m_font->measureText("M") : 9.5f; }
float TextEdit::lineHeight() const { return m_font ? m_font->lineHeight() : 16.0f; }

void TextEdit::insertText(const std::string& str) {
    m_text.insert(m_cursorPos, str); m_cursorPos += str.size();
    if (m_onTextChanged) m_onTextChanged(m_text);
}
void TextEdit::deleteBackward() {
    if (m_cursorPos > 0) { m_text.erase(m_cursorPos - 1, 1); --m_cursorPos; if (m_onTextChanged) m_onTextChanged(m_text); }
}
void TextEdit::deleteForward() {
    if (m_cursorPos < m_text.size()) { m_text.erase(m_cursorPos, 1); if (m_onTextChanged) m_onTextChanged(m_text); }
}
void TextEdit::moveCursorH(int dir) {
    if (dir < 0 && m_cursorPos > 0) --m_cursorPos;
    else if (dir > 0 && m_cursorPos < m_text.size()) ++m_cursorPos;
}

// Wrapped lines using font measurement
std::vector<std::string> TextEdit::wrappedLines(float maxWidth) const {
    std::vector<std::string> result;
    if (!m_font || maxWidth <= 0) {
        std::string cur;
        for (char c : m_text) { if (c == '\n') { result.push_back(cur); cur.clear(); } else cur += c; }
        result.push_back(cur);
        return result;
    }
    std::string cur;
    for (char c : m_text) {
        if (c == '\n') { result.push_back(cur); cur.clear(); continue; }
        std::string test = cur + c;
        if (m_font->measureText(test) > maxWidth && !cur.empty()) {
            result.push_back(cur); cur.clear(); cur += c;
        } else { cur += c; }
    }
    result.push_back(cur);
    return result;
}

void TextEdit::moveCursorV(int dir) {
    float maxW = m_bounds.width > m_padding.horizontal() ? m_bounds.width - m_padding.horizontal() : 400;
    auto lines = wrappedLines(maxW);
    size_t cl = 0, cc = 0;
    cursorToLineCol(lines, m_text, m_cursorPos, cl, cc);
    if (dir < 0 && cl > 0) m_cursorPos = lineColToCursor(lines, m_text, cl - 1, cc);
    else if (dir > 0 && cl + 1 < lines.size()) m_cursorPos = lineColToCursor(lines, m_text, cl + 1, cc);
}

void TextEdit::moveCursorTo(float x, float y) {
    if (!m_font) { m_cursorPos = static_cast<size_t>(std::max(0.0f, x / 9.5f)); return; }
    float maxW = m_bounds.width > m_padding.horizontal() ? m_bounds.width - m_padding.horizontal() : 400;
    auto lines = wrappedLines(maxW);
    float lh = lineHeight();
    size_t line = static_cast<size_t>(std::max(0.0f, (y + m_scrollY) / lh));
    if (line >= lines.size()) line = lines.size() > 0 ? lines.size() - 1 : 0;
    if (lines.empty()) { m_cursorPos = 0; return; }

    const std::string& ln = lines[line];
    size_t lo = 0, hi = ln.size();
    while (lo < hi) {
        size_t mid = (lo + hi + 1) / 2;
        if (m_font->measureText(ln.substr(0, mid)) <= x) lo = mid;
        else hi = mid - 1;
    }
    m_cursorPos = lineColToCursor(lines, m_text, line, lo);
}

Size TextEdit::measure(const Size&) const { return {300, 80 + m_padding.vertical()}; }
void TextEdit::layout() {}

void TextEdit::paint(Painter& painter) {
    Rect r = screenRect();
    // Background fills ENTIRE bounds (shows actual hit area)
    painter.drawRect(r, m_focused ? Color::fromHex(0xFF2A2A2A) : Color::fromHex(0xFF1E1E1E));
    painter.drawRectOutline(r, m_focused ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF444444),
                            m_focused ? 2.0f : 1.0f);

    Rect textArea = r.insetBy(m_padding.left, m_padding.top);
    auto lines = wrappedLines(textArea.width);
    float lh = lineHeight();

    const auto& displayLines = (!m_text.empty() || m_focused) ? lines
                                : std::vector<std::string>{m_placeholder};

    size_t cl = 0, cc = 0;
    cursorToLineCol(lines, m_text, m_cursorPos, cl, cc);

    painter.pushClip(textArea);
    float y = textArea.y - m_scrollY;
    Color textCol = m_focused ? Color::fromHex(0xFFFFFFFF)
                   : (!m_text.empty() ? Color::fromHex(0xFFCCCCCC) : Color::fromHex(0xFF666666));

    for (size_t i = 0; i < displayLines.size(); ++i) {
        if (y + lh < textArea.y) { y += lh; continue; }
        if (y > textArea.y + textArea.height) break;
        painter.drawText({textArea.x - m_scrollX, y, textArea.width + m_scrollX, lh},
                         displayLines[i], textCol, TextAlign::Left);
        y += lh;
    }

    if (m_focused && painter.cursorVisible()) {
        float cursorY = textArea.y + cl * lh - m_scrollY;
        std::string prefix = cl < lines.size() ? lines[cl].substr(0, cc) : "";
        float cursorX = textArea.x + painter.measureTextWidth(prefix) - m_scrollX;

        if (cursorY >= textArea.y && cursorY < textArea.y + textArea.height)
            painter.drawRect({cursorX, cursorY, 2, lh}, Color::fromHex(0xFFFFFFFF));

        if (cl * lh - m_scrollY + lh > textArea.height) m_scrollY = cl * lh - textArea.height + lh;
        if (cl * lh < m_scrollY) m_scrollY = cl * lh;
        float msy = std::max(0.0f, lines.size() * lh - textArea.height);
        if (m_scrollY < 0) m_scrollY = 0;
        if (m_scrollY > msy) m_scrollY = msy;
    }
    painter.popClip();
}

Widget* TextEdit::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x >= 0 && localPoint.y >= 0 &&
        localPoint.x < m_bounds.width && localPoint.y < m_bounds.height)
        return this;
    return nullptr;
}

bool TextEdit::onMouseDown(MouseEvent& e) {
    if (e.button == 0) {
        moveCursorTo(e.localPos.x - m_padding.left, e.localPos.y - m_padding.top);
        return true;
    }
    return false;
}
bool TextEdit::onMouseUp(MouseEvent&) { return false; }
bool TextEdit::onMouseMove(MouseEvent&) { return false; }

bool TextEdit::onKeyDown(KeyEvent& e) {
    if (e.type != KeyEvent::Down && e.type != KeyEvent::Repeat) return false;
    switch (e.key) {
    case 259: deleteBackward(); return true;
    case 261: deleteForward(); return true;
    case 263: if (e.type == KeyEvent::Down) moveCursorH(-1); return true;
    case 262: if (e.type == KeyEvent::Down) moveCursorH(1); return true;
    case 265: if (e.type == KeyEvent::Down) moveCursorV(-1); return true;
    case 264: if (e.type == KeyEvent::Down) moveCursorV(1); return true;
    case 268: if (e.type == KeyEvent::Down) { m_cursorPos = 0; m_scrollX = 0; m_scrollY = 0; } return true;
    case 269: if (e.type == KeyEvent::Down) m_cursorPos = m_text.size(); return true;
    case 257: insertText("\n"); return true;
    default: break;
    }
    return false;
}

bool TextEdit::onCharInput(CharEvent& e) {
    if (e.codepoint >= 32 && e.codepoint != 127) {
        insertText(std::string(1, static_cast<char>(e.codepoint)));
        return true;
    }
    return false;
}

void TextEdit::onFocusGained() { m_focused = true; }
void TextEdit::onFocusLost() { m_focused = false; }

} // namespace ui

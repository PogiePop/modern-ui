#include "TextInput.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

#include <algorithm>
#include <cctype>

namespace ui {

TextInput::TextInput(const std::string& placeholder)
    : m_placeholder(placeholder) {
    m_focusable = true;
    m_tabIndex = 0;
}

void TextInput::setText(const std::string& text) { m_text = text; m_cursorPos = m_text.size(); }
void TextInput::setOnTextChanged(std::function<void(const std::string&)> cb) { m_onTextChanged = std::move(cb); }
void TextInput::setOnEnter(std::function<void()> cb) { m_onEnter = std::move(cb); }

float TextInput::charWidth() const {
    if (m_font) return m_font->measureText("M"); // average char width from a wide char
    return 9.5f; // fallback for 16px
}

void TextInput::insertText(const std::string& str) {
    m_text.insert(m_cursorPos, str);
    m_cursorPos += str.size();
    if (m_onTextChanged) m_onTextChanged(m_text);
}

void TextInput::deleteBackward() {
    if (m_cursorPos > 0) { m_text.erase(m_cursorPos - 1, 1); --m_cursorPos; if (m_onTextChanged) m_onTextChanged(m_text); }
}

void TextInput::deleteForward() {
    if (m_cursorPos < m_text.size()) { m_text.erase(m_cursorPos, 1); if (m_onTextChanged) m_onTextChanged(m_text); }
}

void TextInput::moveCursor(int dir) {
    if (dir < 0 && m_cursorPos > 0) --m_cursorPos;
    else if (dir > 0 && m_cursorPos < m_text.size()) ++m_cursorPos;
}

size_t TextInput::posAtX(const std::string& prefix, float x) const {
    if (m_font) {
        // Binary search for the closest character position
        size_t lo = 0, hi = prefix.size();
        while (lo < hi) {
            size_t mid = (lo + hi + 1) / 2;
            if (m_font->measureText(prefix.substr(0, mid)) <= x) lo = mid;
            else hi = mid - 1;
        }
        return lo;
    }
    // Fallback
    float cw = charWidth();
    size_t pos = static_cast<size_t>(std::max(0.0f, x / cw));
    if (pos > prefix.size()) pos = prefix.size();
    return pos;
}

Size TextInput::measure(const Size&) const {
    return {200, 18.0f + m_padding.vertical()};
}

void TextInput::paint(Painter& painter) {
    Rect r = screenRect();

    Color bgColor = m_focused ? Color::fromHex(0xFF2A2A2A) : Color::fromHex(0xFF1E1E1E);
    painter.drawRect(r, bgColor);

    Color borderColor = m_focused ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF444444);
    painter.drawRectOutline(r, borderColor, m_focused ? 2.0f : 1.0f);

    Rect textArea = r.insetBy(m_padding.left, m_padding.top);

    const std::string& displayText = (!m_text.empty() || m_focused) ? m_text : m_placeholder;
    Color textColor = m_focused ? Color::fromHex(0xFFFFFFFF)
                     : (!m_text.empty() ? Color::fromHex(0xFFCCCCCC) : Color::fromHex(0xFF666666));

    painter.pushClip(textArea);

    Rect scrolledArea = textArea;
    scrolledArea.x -= m_scrollX;
    scrolledArea.width += m_scrollX;
    painter.drawText(scrolledArea, displayText, textColor, TextAlign::Left);

    if (m_focused && painter.cursorVisible()) {
        float cursorX = textArea.x + painter.measureTextWidth(m_text.substr(0, m_cursorPos)) - m_scrollX;

        float textW = painter.measureTextWidth(m_text);
        float maxScroll = textW - textArea.width + 4;
        if (maxScroll < 0) maxScroll = 0;
        if (cursorX > textArea.x + textArea.width - 2)
            m_scrollX += cursorX - (textArea.x + textArea.width) + 4;
        else if (cursorX < textArea.x && m_scrollX > 0)
            m_scrollX -= (textArea.x - cursorX) + 16;
        if (m_scrollX < 0) m_scrollX = 0;
        if (m_scrollX > maxScroll) m_scrollX = maxScroll;

        cursorX = textArea.x + painter.measureTextWidth(m_text.substr(0, m_cursorPos)) - m_scrollX;
        painter.drawRect({cursorX, textArea.y, 2, textArea.height}, Color::fromHex(0xFFFFFFFF));
    }

    painter.popClip();
}

bool TextInput::onMouseDown(MouseEvent& event) {
    if (event.button == 0) {
        float relX = event.localPos.x - m_padding.left + m_scrollX;
        // Find closest char using font binary search
        m_cursorPos = posAtX(m_text, relX);
        return true;
    }
    return false;
}

bool TextInput::onMouseUp(MouseEvent&) { return false; }
bool TextInput::onMouseMove(MouseEvent&) { return false; }

bool TextInput::onKeyDown(KeyEvent& event) {
    if (event.type != KeyEvent::Down && event.type != KeyEvent::Repeat) return false;
    switch (event.key) {
    case 259: deleteBackward(); return true;
    case 261: deleteForward(); return true;
    case 263: if (event.type == KeyEvent::Down) moveCursor(-1); return true;
    case 262: if (event.type == KeyEvent::Down) moveCursor(1); return true;
    case 268: if (event.type == KeyEvent::Down) m_cursorPos = 0; return true;
    case 269: if (event.type == KeyEvent::Down) m_cursorPos = m_text.size(); return true;
    case 257: case 335: if (event.type == KeyEvent::Down && m_onEnter) m_onEnter(); return true;
    default: break;
    }
    return false;
}

bool TextInput::onCharInput(CharEvent& event) {
    if (event.codepoint >= 32 && event.codepoint != 127) {
        insertText(std::string(1, static_cast<char>(event.codepoint)));
        return true;
    }
    return false;
}

void TextInput::onFocusGained() { m_focused = true; }
void TextInput::onFocusLost() { m_focused = false; }

} // namespace ui

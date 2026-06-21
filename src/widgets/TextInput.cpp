#include "TextInput.hpp"
#include "ContextMenu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
#include "utils/Clipboard.hpp"
#include <algorithm>
#include <cctype>

namespace ui {

TextInput::TextInput(const std::string& placeholder)
    : m_placeholder(placeholder) {
    m_focusable = true;
    m_tabIndex = 0;
    fprintf(stderr, "[TextInput] created placeholder=\"%s\" — right-click for menu, drag to select\n",
            placeholder.c_str());
}

TextInput::~TextInput() = default;

void TextInput::setText(const std::string& text) {
    m_text = text;
    m_cursorPos = text.size();
    clearSelection();
}

void TextInput::setOnTextChanged(std::function<void(const std::string&)> cb) { m_onTextChanged = std::move(cb); }
void TextInput::setOnEnter(std::function<void()> cb) { m_onEnter = std::move(cb); }

float TextInput::charWidth() const {
    if (m_font) return m_font->measureText("M");
    return 9.5f;
}

// ── Selection ──
bool TextInput::hasSelection() const { return m_selStart != m_selEnd; }

std::string TextInput::selectedText() const {
    if (!hasSelection()) return "";
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    return m_text.substr(s, e - s);
}

void TextInput::selectAll() { m_selStart = 0; m_selEnd = m_text.size(); }

void TextInput::clearSelection() { m_selStart = m_selEnd = m_cursorPos; }

void TextInput::copy() {
    if (!hasSelection()) return;
    Clipboard::set(selectedText());
}
void TextInput::cut() { copy(); deleteSelection(); }
void TextInput::paste() {
    std::string clip = Clipboard::get();
    if (!clip.empty()) { deleteSelection(); insertText(clip); }
}
void TextInput::pushUndo() {
    if (m_undoStack.empty() || m_undoStack[m_undoIndex] != m_text) {
        m_undoStack.resize(m_undoIndex + 1);
        m_undoStack.push_back(m_text);
        if (m_undoStack.size() > MAX_UNDO) { m_undoStack.erase(m_undoStack.begin()); }
        m_undoIndex = m_undoStack.size() - 1;
    }
}
void TextInput::undo() {
    if (m_undoIndex > 0) {
        if (m_undoStack[m_undoIndex] != m_text) pushUndo();
        m_undoIndex--;
        m_text = m_undoStack[m_undoIndex];
        m_cursorPos = m_text.size();
        clearSelection();
        textChanged();
    }
}
void TextInput::redo() {
    if (m_undoIndex + 1 < m_undoStack.size()) {
        m_undoIndex++;
        m_text = m_undoStack[m_undoIndex];
        m_cursorPos = m_text.size();
        clearSelection();
        textChanged();
    }
}

void TextInput::deleteSelection() {
    if (!hasSelection()) return;
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    deleteRange(s, e);
}

void TextInput::deleteRange(size_t from, size_t to) {
    if (from >= to || from >= m_text.size()) return;
    if (to > m_text.size()) to = m_text.size();
    pushUndo();
    m_text.erase(from, to - from);
    m_cursorPos = from;
    clearSelection();
    textChanged();
}

// ── Editing ──
void TextInput::insertText(const std::string& str) {
    pushUndo();
    m_text.insert(m_cursorPos, str);
    m_cursorPos += str.size();
    clearSelection();
    textChanged();
}

// UTF-8: find start of codepoint at or before byte position
static size_t utf8PrevCharStart(const std::string& s, size_t pos) {
    if (pos == 0) return 0;
    size_t i = pos - 1;
    while (i > 0 && (s[i] & 0xC0) == 0x80) i--;
    return i;
}
// UTF-8: find start of next codepoint after byte position
static size_t utf8NextCharStart(const std::string& s, size_t pos) {
    if (pos >= s.size()) return s.size();
    size_t i = pos;
    while (i < s.size() && (s[i] & 0xC0) == 0x80) i++;
    if (i < s.size()) {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x80) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
    }
    return i;
}

void TextInput::deleteBackward() {
    if (hasSelection()) { deleteSelection(); return; }
    if (m_cursorPos > 0) {
        size_t prev = utf8PrevCharStart(m_text, m_cursorPos);
        pushUndo();
        m_text.erase(prev, m_cursorPos - prev);
        m_cursorPos = prev;
        clearSelection();
        textChanged();
    }
}

void TextInput::deleteForward() {
    if (hasSelection()) { deleteSelection(); return; }
    if (m_cursorPos < m_text.size()) {
        size_t next = utf8NextCharStart(m_text, m_cursorPos);
        pushUndo();
        m_text.erase(m_cursorPos, next - m_cursorPos);
        clearSelection();
        textChanged();
    }
}

// UTF-8: move cursor by N codepoints (not bytes)
static size_t utf8CursorMove(const std::string& s, size_t bytePos, int cpDelta) {
    if (cpDelta == 0) return bytePos;
    // Convert byte offset to codepoint count
    size_t cpIdx = 0;
    for (size_t i = 0; i < bytePos && i < s.size(); i++)
        if ((s[i] & 0xC0) != 0x80) cpIdx++;
    int newCp = (int)cpIdx + cpDelta;
    if (newCp < 0) return 0;
    size_t n = 0;
    for (size_t j = 0; j < s.size(); j++) {
        if ((s[j] & 0xC0) != 0x80) {
            if ((size_t)newCp == n) return j;
            n++;
        }
    }
    return s.size();
}

void TextInput::moveCursor(int dir) {
    m_cursorPos = utf8CursorMove(m_text, m_cursorPos, dir);
    clearSelection();
}

void TextInput::setCursorPos(size_t pos, bool extendSelection) {
    if (extendSelection) {
        m_selEnd = pos;
    } else {
        m_cursorPos = pos;
        clearSelection();
    }
}

size_t TextInput::posAtX(const std::string& prefix, float x) const {
    if (m_font) {
        size_t lo = 0, hi = prefix.size();
        while (lo < hi) {
            size_t mid = (lo + hi + 1) / 2;
            if (m_font->measureText(prefix.substr(0, mid)) <= x) lo = mid;
            else hi = mid - 1;
        }
        return lo;
    }
    float cw = charWidth();
    size_t pos = static_cast<size_t>(std::max(0.0f, x / cw));
    if (pos > prefix.size()) pos = prefix.size();
    return pos;
}

void TextInput::textChanged() { if (m_onTextChanged) m_onTextChanged(m_text); }

// ── Context menu ──
void TextInput::ensureContextMenu() {
    if (!m_contextMenu) m_contextMenu = std::make_unique<ContextMenu>();
}

void TextInput::buildContextMenuItems() {
    if (!m_contextMenu) return;
    m_contextMenu->clearItems();
    bool canUndo = m_undoIndex > 0;
    bool canRedo = m_undoIndex + 1 < m_undoStack.size();
    m_contextMenu->addItem("Undo", [this]() { undo(); }, "Ctrl+Z", canUndo);
    m_contextMenu->addItem("Redo", [this]() { redo(); }, "Ctrl+Y", canRedo);
    m_contextMenu->addSeparator();
    m_contextMenu->addItem("Cut", [this]() { cut(); }, "Ctrl+X", hasSelection());
    m_contextMenu->addItem("Copy", [this]() { copy(); }, "Ctrl+C", hasSelection());
    m_contextMenu->addItem("Paste", [this]() { paste(); }, "Ctrl+V", Clipboard::available());
    m_contextMenu->addItem("Delete", [this]() { deleteSelection(); }, "Del", hasSelection());
    m_contextMenu->addSeparator();
    m_contextMenu->addItem("Select All", [this]() { selectAll(); }, "Ctrl+A");
    for (auto& ci : m_customItems) m_contextMenu->addItem(ci.first, ci.second);
}

void TextInput::addContextMenuItem(const std::string& label, std::function<void()> action, bool enabled) {
    m_customItems.push_back({label, std::move(action)});
}

void TextInput::showContextMenu(float x, float y) {
    ensureContextMenu();
    buildContextMenuItems();
    m_contextMenu->setFont(m_font);
    if (m_theme) m_contextMenu->setTheme(m_theme);
    m_contextMenu->show(x, y, this);
}

// ── Measure ──
Size TextInput::measure(const Size&) const {
    return {200, 18.0f + m_padding.vertical()};
}

// ── Paint ──
void TextInput::paint(Painter& painter) {
    Rect r = screenRect();

    Color bgColor = m_focused ? Color::fromHex(0xFF2A2A2A) : Color::fromHex(0xFF1E1E1E);
    Color borderColor = m_focused ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF444444);
    Color textColor = Color::fromHex(0xFFCCCCCC);
    Color placeholderColor = Color::fromHex(0xFF666666);
    Color selBg = Color::fromHex(0x663B82F6);
    Color cursorColor = Color::fromHex(0xFFFFFFFF);

    if (m_theme) {
        bgColor = m_focused ? m_theme->color(ColorRole::BgTertiary) : m_theme->color(ColorRole::Surface);
        borderColor = m_focused ? m_theme->color(ColorRole::BorderFocused) : m_theme->color(ColorRole::Border);
        textColor = m_theme->color(ColorRole::Text);
        placeholderColor = m_theme->color(ColorRole::TextDisabled);
        selBg = m_theme->color(ColorRole::Primary); selBg.a = 0.3f;
    }

    painter.drawRoundedRect(r, bgColor, 6);
    painter.drawRectOutline(r, borderColor, m_focused ? 2.0f : 1.0f);

    Rect textArea = r.insetBy(m_padding.left, m_padding.top);
    painter.pushClip(textArea);

    const std::string& displayText = (!m_text.empty() || m_focused) ? m_text : m_placeholder;
    Color displayColor = (!m_text.empty() || m_focused) ? textColor : placeholderColor;

    // Measure selection visual position
    float selStartX = 0, selEndX = 0;
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    if (s != e) {
        std::string prefix = displayText.substr(0, s);
        std::string selPart = displayText.substr(s, e - s);
        selStartX = textArea.x + painter.measureTextWidth(prefix) - m_scrollX;
        selEndX = selStartX + painter.measureTextWidth(selPart);
    }

    // Selection highlight
    if (s != e && m_focused) {
        painter.drawRect({selStartX, textArea.y, selEndX - selStartX, textArea.height}, selBg);
    }

    // Text
    Rect scrolledArea = textArea;
    scrolledArea.x -= m_scrollX;
    scrolledArea.width += m_scrollX;
    painter.drawText(scrolledArea, displayText, displayColor, TextAlign::Left);

    // Cursor
    if (m_focused && painter.cursorVisible() && !hasSelection()) {
        float cursorX = textArea.x + painter.measureTextWidth(displayText.substr(0, m_cursorPos)) - m_scrollX;
        float textW = painter.measureTextWidth(displayText);
        float maxScroll = textW - textArea.width + 4;
        if (maxScroll < 0) maxScroll = 0;
        if (cursorX > textArea.x + textArea.width - 2)
            m_scrollX += cursorX - (textArea.x + textArea.width) + 4;
        else if (cursorX < textArea.x && m_scrollX > 0)
            m_scrollX -= (textArea.x - cursorX) + 16;
        if (m_scrollX < 0) m_scrollX = 0;
        if (m_scrollX > maxScroll) m_scrollX = maxScroll;

        cursorX = textArea.x + painter.measureTextWidth(displayText.substr(0, m_cursorPos)) - m_scrollX;
        painter.drawRect({cursorX, textArea.y, 2, textArea.height}, cursorColor);
    }

    painter.popClip();

    // Clear X button (when text is non-empty)
    if (!m_text.empty()) {
        float cx = r.x + r.width - 16;
        float cy = r.y + r.height * 0.5f;
        Color xCol{0.5f, 0.5f, 0.55f, 1};
        if (m_theme) xCol = m_theme->color(ColorRole::TextDisabled);
        // Draw a small "x" marker
        painter.drawRect({cx - 4, cy - 1, 8, 2}, xCol);
        painter.drawRect({cx - 1, cy - 4, 2, 8}, xCol);
    }
}

// ── Mouse ──
bool TextInput::onMouseDown(MouseEvent& e) {
    // Check for clear button click (small X at right edge when text is non-empty)
    bool hasClear = !m_text.empty();
    float clearX = m_bounds.width - 20;
    if (hasClear && e.button == 0 && e.localPos.x >= clearX) {
        setText(""); // clear all text
        return true;
    }

    if (e.button == 0) {
        float relX = e.localPos.x - m_padding.left + m_scrollX;
        size_t newPos = posAtX(m_text, relX);
        bool extend = (e.mods & 0x1) != 0; // Shift
        setCursorPos(newPos, extend);
        if (!extend) { m_selStart = m_selEnd = newPos; }
        m_cursorPos = newPos;
        m_dragging = true;
        return true;
    }
    if (e.button == 1) {
        showContextMenu(e.globalPos.x, e.globalPos.y);
        return true;
    }
    return false;
}

bool TextInput::onMouseUp(MouseEvent& e) {
    if (e.button == 0) { m_dragging = false; return true; }
    return false;
}

bool TextInput::onMouseMove(MouseEvent& e) {
    if (m_dragging) {
        float relX = e.localPos.x - m_padding.left + m_scrollX;
        size_t newPos = posAtX(m_text, relX);
        m_cursorPos = newPos;
        m_selEnd = newPos;
        return true;
    }
    return false;
}

// ── Keyboard ──
bool TextInput::onKeyDown(KeyEvent& event) {
    if (event.type != KeyEvent::Down && event.type != KeyEvent::Repeat) return false;

    // Ctrl/Cmd shortcuts
    if (event.mods & 0x2) {
        if (event.isUndo()) { undo(); return true; }
        if (event.isRedo()) { redo(); return true; }
        if (event.isCopy()) { copy(); return true; }
        if (event.isCut()) { cut(); return true; }
        if (event.isPaste()) { paste(); return true; }
        if (event.isSelectAll()) { selectAll(); return true; }
        return false;
    }

    // Navigation with shift = extend selection
    bool shift = (event.mods & 0x1) != 0;

    if (event.isBackspace()) { deleteBackward(); return true; }
    if (event.isDelete()) { deleteForward(); return true; }
    if (event.isLeft()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursor(-1);
        if (shift) m_selEnd = m_cursorPos;
        else clearSelection();
        return true;
    }
    if (event.isRight()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursor(1);
        if (shift) m_selEnd = m_cursorPos;
        else clearSelection();
        return true;
    }
    if (event.isHome()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        m_cursorPos = 0;
        if (shift) m_selEnd = 0; else clearSelection();
        return true;
    }
    if (event.isEnd()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        m_cursorPos = m_text.size();
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (event.isEnter() && m_onEnter) { m_onEnter(); return true; }
    return false;
}

bool TextInput::onCharInput(CharEvent& event) {
    if (event.isPrintable()) {
        deleteSelection();
        insertText(codepointToUtf8(event.codepoint));
        return true;
    }
    return false;
}

void TextInput::onFocusGained() { m_focused = true; }
void TextInput::onFocusLost() { m_focused = false; }

} // namespace ui

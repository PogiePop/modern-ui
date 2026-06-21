#include "TextEdit.hpp"
#include "ContextMenu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
#include "utils/Clipboard.hpp"
#include <algorithm>
#include <vector>
#include <cstdio>

namespace ui {

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

// UTF-8 helper: count codepoints in a string
static size_t utf8Len(const std::string& s) {
    size_t n = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if ((s[i] & 0xC0) != 0x80) n++;
    }
    return n;
}
// Byte offset of n-th codepoint
static size_t utf8Offset(const std::string& s, size_t cpIdx) {
    size_t n = 0;
    for (size_t i = 0; i < s.size(); i++) {
        if ((s[i] & 0xC0) != 0x80) {
            if (n == cpIdx) return i;
            n++;
        }
    }
    return s.size();
}
// Move by N codepoints from byte offset
static size_t utf8Move(const std::string& s, size_t bytePos, int cpDelta) {
    if (cpDelta == 0) return bytePos;
    // Convert to cp index
    size_t cpIdx = 0;
    size_t i = 0;
    for (; i < bytePos && i < s.size(); i++)
        if ((s[i] & 0xC0) != 0x80) cpIdx++;
    int newCp = (int)cpIdx + cpDelta;
    if (newCp < 0) return 0;
    size_t n = 0, j = 0;
    for (; j < s.size(); j++) {
        if ((s[j] & 0xC0) != 0x80) {
            if ((size_t)newCp == n) return j;
            n++;
        }
    }
    return s.size();
}

// ── Line helpers ──

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

// ── Constructor ──

TextEdit::TextEdit(const std::string& placeholder) : m_placeholder(placeholder) {
    m_focusable = true; m_tabIndex = 0;
    fprintf(stderr, "[TextEdit] created — right-click for menu, drag to select\n");
}
TextEdit::~TextEdit() = default;

void TextEdit::setText(const std::string& text) { m_text = text; m_cursorPos = text.size(); clearSelection(); }
void TextEdit::setOnTextChanged(std::function<void(const std::string&)> cb) { m_onTextChanged = std::move(cb); }

float TextEdit::charWidth() const { return m_font ? m_font->measureText("M") : 9.5f; }
float TextEdit::lineHeight() const { return m_font ? m_font->lineHeight() : 16.0f; }

// ── Selection ──
bool TextEdit::hasSelection() const { return m_selStart != m_selEnd; }
std::string TextEdit::selectedText() const {
    if (!hasSelection()) return "";
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    return m_text.substr(s, e - s);
}
void TextEdit::selectAll() { m_selStart = 0; m_selEnd = m_text.size(); }
void TextEdit::clearSelection() { m_selStart = m_selEnd = m_cursorPos; }

void TextEdit::copy() { if (hasSelection()) Clipboard::set(selectedText()); }
void TextEdit::cut() { copy(); deleteSelection(); }
void TextEdit::paste() {
    std::string clip = Clipboard::get();
    if (!clip.empty()) { deleteSelection(); insertText(clip); }
}
void TextEdit::pushUndo() {
    if (m_undoStack.empty() || m_undoStack[m_undoIndex] != m_text) {
        m_undoStack.resize(m_undoIndex + 1);
        m_undoStack.push_back(m_text);
        if (m_undoStack.size() > MAX_UNDO) { m_undoStack.erase(m_undoStack.begin()); }
        m_undoIndex = m_undoStack.size() - 1;
    }
}
void TextEdit::undo() {
    if (m_undoIndex > 0) {
        if (m_undoStack[m_undoIndex] != m_text) pushUndo();
        m_undoIndex--;
        m_text = m_undoStack[m_undoIndex];
        m_cursorPos = m_text.size(); clearSelection(); textChanged();
    }
}
void TextEdit::redo() {
    if (m_undoIndex + 1 < m_undoStack.size()) {
        m_undoIndex++;
        m_text = m_undoStack[m_undoIndex];
        m_cursorPos = m_text.size(); clearSelection(); textChanged();
    }
}
void TextEdit::deleteSelection() {
    if (!hasSelection()) return;
    size_t s = std::min(m_selStart, m_selEnd);
    size_t e = std::max(m_selStart, m_selEnd);
    deleteRange(s, e);
}
void TextEdit::deleteRange(size_t from, size_t to) {
    if (from >= to || from >= m_text.size()) return;
    if (to > m_text.size()) to = m_text.size();
    from = utf8PrevCharStart(m_text, from);
    while (to < m_text.size() && (m_text[to] & 0xC0) == 0x80) to++;
    if (from >= to) return;
    pushUndo();
    m_text.erase(from, to - from);
    m_cursorPos = from;
    clearSelection();
    textChanged();
}

// ── Editing ──
void TextEdit::insertText(const std::string& str) {
    pushUndo();
    m_text.insert(m_cursorPos, str); m_cursorPos += str.size(); clearSelection(); textChanged();
}
void TextEdit::deleteBackward() {
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
void TextEdit::deleteForward() {
    if (hasSelection()) { deleteSelection(); return; }
    if (m_cursorPos < m_text.size()) {
        size_t next = utf8NextCharStart(m_text, m_cursorPos);
        pushUndo();
        m_text.erase(m_cursorPos, next - m_cursorPos);
        clearSelection();
        textChanged();
    }
}
void TextEdit::moveCursorH(int dir) {
    if (dir < 0) m_cursorPos = utf8Move(m_text, m_cursorPos, -1);
    else if (dir > 0) m_cursorPos = utf8Move(m_text, m_cursorPos, 1);
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
    // Build codepoint offset table for O(1) bytePos lookups
    std::vector<size_t> cpOffsets;
    for (size_t i = 0; i < ln.size(); i++) if ((ln[i] & 0xC0) != 0x80) cpOffsets.push_back(i);
    size_t cpCount = cpOffsets.size();
    size_t cpLo = 0, cpHi = cpCount;
    while (cpLo < cpHi) {
        size_t cpMid = (cpLo + cpHi + 1) / 2;
        size_t bytePos = (cpMid < cpCount) ? cpOffsets[cpMid] : ln.size();
        if (m_font->measureText(ln.substr(0, bytePos)) <= x) cpLo = cpMid;
        else cpHi = cpMid - 1;
    }
    size_t lo = (cpLo < cpCount) ? cpOffsets[cpLo] : ln.size();
    m_cursorPos = lineColToCursor(lines, m_text, line, lo);
}
void TextEdit::textChanged() { if (m_onTextChanged) m_onTextChanged(m_text); }

// ── Context menu ──
void TextEdit::ensureContextMenu() { if (!m_contextMenu) m_contextMenu = std::make_unique<ContextMenu>(); }
void TextEdit::buildContextMenuItems() {
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
void TextEdit::addContextMenuItem(const std::string& label, std::function<void()> action, bool enabled) {
    m_customItems.push_back({label, std::move(action)});
}
void TextEdit::showContextMenu(float x, float y) {
    ensureContextMenu(); buildContextMenuItems();
    m_contextMenu->setFont(m_font);
    if (m_theme) m_contextMenu->setTheme(m_theme);
    m_contextMenu->show(x, y, this);
}

// ── Wrapped lines ──
std::vector<std::string> TextEdit::wrappedLines(float maxWidth) const {
    std::vector<std::string> result;
    if (!m_font || maxWidth <= 0) {
        std::string cur;
        const char* p = m_text.c_str();
        while (*p) {
            uint32_t cp = decodeUtf8(p);
            if (cp == '\n') { result.push_back(cur); cur.clear(); }
            else cur += codepointToUtf8(cp);
        }
        result.push_back(cur);
        return result;
    }
    std::string cur;
    const char* p = m_text.c_str();
    while (*p) {
        const char* saved = p;
        uint32_t cp = decodeUtf8(p);
        if (cp == '\n') { result.push_back(cur); cur.clear(); continue; }
        std::string ch = codepointToUtf8(cp);
        std::string test = cur + ch;
        if (m_font->measureText(test) > maxWidth && !cur.empty()) {
            result.push_back(cur);
            cur.clear();
            cur = ch;
        } else {
            cur += ch;
        }
    }
    if (!cur.empty()) result.push_back(cur);
    if (result.empty()) result.push_back("");
    return result;
}

// ── Measure / Layout ──
Size TextEdit::measure(const Size&) const { return {300, 80 + m_padding.vertical()}; }
void TextEdit::layout() {}

// ── Paint ──
void TextEdit::paint(Painter& painter) {
    Rect r = screenRect();
    Color bgColor = m_focused ? Color::fromHex(0xFF2A2A2A) : Color::fromHex(0xFF1E1E1E);
    Color borderColor = m_focused ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF444444);
    Color textCol = Color::fromHex(0xFFCCCCCC);
    Color placeholderCol = Color::fromHex(0xFF666666);
    Color selBg = Color::fromHex(0x663B82F6);
    Color cursorCol = Color::fromHex(0xFFFFFFFF);

    if (m_theme) {
        bgColor = m_focused ? m_theme->color(ColorRole::BgTertiary) : m_theme->color(ColorRole::Surface);
        borderColor = m_focused ? m_theme->color(ColorRole::BorderFocused) : m_theme->color(ColorRole::Border);
        textCol = m_theme->color(ColorRole::Text);
        placeholderCol = m_theme->color(ColorRole::TextDisabled);
        selBg = m_theme->color(ColorRole::Primary); selBg.a = 0.3f;
    }

    painter.drawRoundedRect(r, bgColor, 6);
    painter.drawRectOutline(r, borderColor, m_focused ? 2.0f : 1.0f);

    Rect textArea = r.insetBy(m_padding.left, m_padding.top);
    auto lines = wrappedLines(textArea.width);
    float lh = lineHeight();

    const auto& displayLines = (!m_text.empty() || m_focused) ? lines : std::vector<std::string>{m_placeholder};
    size_t cl = 0, cc = 0;
    cursorToLineCol(lines, m_text, m_cursorPos, cl, cc);

    // Selection range in lines
    size_t slLine = 0, slCol = 0, elLine = 0, elCol = 0;
    size_t ss = std::min(m_selStart, m_selEnd), ee = std::max(m_selStart, m_selEnd);
    cursorToLineCol(lines, m_text, ss, slLine, slCol);
    cursorToLineCol(lines, m_text, ee, elLine, elCol);

    painter.pushClip(textArea);
    float y = textArea.y - m_scrollY;
    Color displayCol = (!m_text.empty() || m_focused) ? textCol : placeholderCol;

    for (size_t i = 0; i < displayLines.size(); ++i) {
        if (y + lh < textArea.y) { y += lh; continue; }
        if (y > textArea.y + textArea.height) break;

        // Selection highlight for this line
        if (hasSelection() && m_focused && i >= slLine && i <= elLine) {
            size_t lineStart = (i == slLine) ? slCol : 0;
            size_t lineEnd = (i == elLine) ? elCol : displayLines[i].size();
            float sx = textArea.x + painter.measureTextWidth(displayLines[i].substr(0, lineStart)) - m_scrollX;
            float sw = painter.measureTextWidth(displayLines[i].substr(lineStart, lineEnd - lineStart));
            painter.drawRect({sx, y, sw, lh}, selBg);
        }

        painter.drawText({textArea.x - m_scrollX, y, textArea.width + m_scrollX, lh},
                         displayLines[i], displayCol, TextAlign::Left);
        y += lh;
    }

    // Cursor
    if (m_focused && painter.cursorVisible() && !hasSelection()) {
        float cursorY = textArea.y + cl * lh - m_scrollY;
        std::string prefix = cl < lines.size() ? lines[cl].substr(0, cc) : "";
        float cursorX = textArea.x + painter.measureTextWidth(prefix) - m_scrollX;
        if (cursorY >= textArea.y && cursorY < textArea.y + textArea.height)
            painter.drawRect({cursorX, cursorY, 2, lh}, cursorCol);

        if (cl * lh - m_scrollY + lh > textArea.height) m_scrollY = cl * lh - textArea.height + lh;
        if (cl * lh < m_scrollY) m_scrollY = cl * lh;
        float msy = std::max(0.0f, lines.size() * lh - textArea.height);
        if (m_scrollY < 0) m_scrollY = 0;
        if (m_scrollY > msy) m_scrollY = msy;
    }
    painter.popClip();

    // Clear X button
    if (!m_text.empty()) {
        float cx = r.x + r.width - 16, cy = r.y + r.height * 0.5f;
        Color xCol{0.5f, 0.5f, 0.55f, 1};
        if (m_theme) xCol = m_theme->color(ColorRole::TextDisabled);
        painter.drawRect({cx - 4, cy - 1, 8, 2}, xCol);
        painter.drawRect({cx - 1, cy - 4, 2, 8}, xCol);
    }
}

// ── HitTest ──
Widget* TextEdit::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x >= 0 && localPoint.y >= 0 &&
        localPoint.x < m_bounds.width && localPoint.y < m_bounds.height)
        return this;
    return nullptr;
}

// ── Mouse ──
bool TextEdit::onMouseDown(MouseEvent& e) {
    // Clear X button
    if (!m_text.empty() && e.button == 0 && e.localPos.x >= m_bounds.width - 20) {
        setText(""); return true;
    }
    if (e.button == 0) {
        moveCursorTo(e.localPos.x - m_padding.left, e.localPos.y - m_padding.top);
        bool extend = (e.mods & 0x1) != 0;
        if (!extend) { m_selStart = m_selEnd = m_cursorPos; }
        else m_selEnd = m_cursorPos;
        m_dragging = true;
        return true;
    }
    if (e.button == 1) {
        showContextMenu(e.globalPos.x, e.globalPos.y);
        return true;
    }
    return false;
}
bool TextEdit::onMouseUp(MouseEvent& e) {
    if (e.button == 0) { m_dragging = false; return true; }
    return false;
}
bool TextEdit::onMouseMove(MouseEvent& e) {
    if (m_dragging) {
        moveCursorTo(e.localPos.x - m_padding.left, e.localPos.y - m_padding.top);
        m_selEnd = m_cursorPos;
        return true;
    }
    return false;
}

// ── Keyboard ──
bool TextEdit::onKeyDown(KeyEvent& e) {
    if (e.type != KeyEvent::Down && e.type != KeyEvent::Repeat) return false;
    if (e.mods & 0x2) {
        if (e.isUndo()) { undo(); return true; }
        if (e.isRedo()) { redo(); return true; }
        if (e.isCopy()) { copy(); return true; }
        if (e.isCut()) { cut(); return true; }
        if (e.isPaste()) { paste(); return true; }
        if (e.isSelectAll()) { selectAll(); return true; }
        return false;
    }
    bool shift = (e.mods & 0x1) != 0;
    if (e.isBackspace()) { deleteBackward(); return true; }
    if (e.isDelete()) { deleteForward(); return true; }
    if (e.isLeft()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursorH(-1);
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (e.isRight()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursorH(1);
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (e.isUp()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursorV(-1);
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (e.isDown()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        moveCursorV(1);
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (e.isHome()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        m_cursorPos = 0; m_scrollX = 0; m_scrollY = 0;
        if (shift) m_selEnd = 0; else clearSelection();
        return true;
    }
    if (e.isEnd()) {
        if (shift && !hasSelection()) m_selStart = m_cursorPos;
        m_cursorPos = m_text.size();
        if (shift) m_selEnd = m_cursorPos; else clearSelection();
        return true;
    }
    if (e.isEnter()) { insertText("\n"); return true; }
    return false;
}
bool TextEdit::onCharInput(CharEvent& e) {
    if (e.isPrintable()) {
        deleteSelection();
        insertText(codepointToUtf8(e.codepoint));
        return true;
    }
    return false;
}
void TextEdit::onFocusGained() { m_focused = true; }
void TextEdit::onFocusLost() { m_focused = false; }

} // namespace ui

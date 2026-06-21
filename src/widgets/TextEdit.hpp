#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "Widget.hpp"

namespace ui {

class Font;
class ContextMenu;

class TextEdit : public Widget {
public:
    explicit TextEdit(const std::string& placeholder = "");
    ~TextEdit() override;

    void setText(const std::string& text);
    const std::string& text() const { return m_text; }
    void setPlaceholder(const std::string& text) { m_placeholder = text; }
    void setOnTextChanged(std::function<void(const std::string&)> cb);
    void setFont(Font* font) { m_font = font; }

    // Selection
    bool hasSelection() const;
    std::string selectedText() const;
    void selectAll();
    void clearSelection();

    // Editing operations
    void copy();
    void cut();
    void paste();
    void deleteSelection();
    void undo();
    void redo();

    // Context menu
    void addContextMenuItem(const std::string& label, std::function<void()> action, bool enabled = true);

    const char* typeName() const override { return "TextEdit"; }

    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;
    Widget* hitTest(Point localPoint) override;
    bool onMouseDown(MouseEvent& event) override;
    bool onMouseUp(MouseEvent& event) override;
    bool onMouseMove(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;
    bool onCharInput(CharEvent& event) override;
    void onFocusGained() override;
    void onFocusLost() override;

private:
    std::string m_text;
    std::string m_placeholder;
    std::function<void(const std::string&)> m_onTextChanged;
    Font* m_font = nullptr;

    size_t m_cursorPos = 0;
    size_t m_selStart = 0, m_selEnd = 0;
    bool m_dragging = false;
    bool m_focused = false;
    float m_scrollX = 0;
    float m_scrollY = 0;
    EdgeInsets m_padding{8, 8, 8, 8};

    std::unique_ptr<ContextMenu> m_contextMenu;
    std::vector<std::pair<std::string, std::function<void()>>> m_customItems;
    std::vector<std::string> m_undoStack;
    size_t m_undoIndex = 0;
    static constexpr size_t MAX_UNDO = 128;
    void pushUndo();

    float charWidth() const;
    float lineHeight() const;

    void insertText(const std::string& str);
    void deleteBackward();
    void deleteForward();
    void deleteRange(size_t from, size_t to);
    void moveCursorH(int dir);
    void moveCursorV(int dir);
    void moveCursorTo(float x, float y);

    std::vector<std::string> wrappedLines(float maxWidth) const;
    void textChanged();

    void ensureContextMenu();
    void showContextMenu(float x, float y);
    void buildContextMenuItems();
};

} // namespace ui

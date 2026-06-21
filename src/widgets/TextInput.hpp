#pragma once

#include <functional>
#include <string>
#include <memory>

#include "Widget.hpp"

namespace ui {

class Font;
class ContextMenu;

class TextInput : public Widget {
public:
    explicit TextInput(const std::string& placeholder = "");
    ~TextInput() override;

    void setText(const std::string& text);
    const std::string& text() const { return m_text; }
    void setPlaceholder(const std::string& text) { m_placeholder = text; }
    void setOnTextChanged(std::function<void(const std::string&)> cb);
    void setOnEnter(std::function<void()> cb);
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

    // Context menu (can add custom items)
    void addContextMenuItem(const std::string& label, std::function<void()> action, bool enabled = true);

    const char* typeName() const override { return "TextInput"; }

    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;
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
    std::function<void()> m_onEnter;
    Font* m_font = nullptr;

    size_t m_cursorPos = 0;
    size_t m_selStart = 0, m_selEnd = 0; // selection range [start, end)
    bool m_dragging = false;
    bool m_focused = false;
    float m_scrollX = 0;
    EdgeInsets m_padding{8, 8, 8, 8};

    // Context menu + undo
    std::unique_ptr<ContextMenu> m_contextMenu;
    std::vector<std::pair<std::string, std::function<void()>>> m_customItems;
    std::vector<std::string> m_undoStack;
    size_t m_undoIndex = 0;
    static constexpr size_t MAX_UNDO = 128;

    void pushUndo();
    void insertText(const std::string& str);
    void deleteBackward();
    void deleteForward();
    void deleteRange(size_t from, size_t to);
    void moveCursor(int dir);
    float charWidth() const;
    size_t posAtX(const std::string& prefix, float x) const;
    void setCursorPos(size_t pos, bool extendSelection);

    void ensureContextMenu();
    void showContextMenu(float x, float y);
    void buildContextMenuItems();
    void textChanged();
};

} // namespace ui

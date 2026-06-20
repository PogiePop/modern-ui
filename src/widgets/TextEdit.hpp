#pragma once

#include <functional>
#include <string>
#include <vector>

#include "Widget.hpp"

namespace ui {

class Font;

class TextEdit : public Widget {
public:
    explicit TextEdit(const std::string& placeholder = "");

    void setText(const std::string& text);
    const std::string& text() const { return m_text; }
    void setPlaceholder(const std::string& text) { m_placeholder = text; }
    void setOnTextChanged(std::function<void(const std::string&)> cb);
    void setFont(Font* font) { m_font = font; }

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
    bool m_focused = false;
    float m_scrollX = 0;
    float m_scrollY = 0;
    EdgeInsets m_padding{8, 8, 8, 8};

    float charWidth() const;
    float lineHeight() const;

    void insertText(const std::string& str);
    void deleteBackward();
    void deleteForward();
    void moveCursorH(int dir);
    void moveCursorV(int dir);
    void moveCursorTo(float x, float y);

    // Line wrapping helpers
    std::vector<std::string> wrappedLines(float maxWidth) const;
    size_t cursorLine() const;
    size_t cursorCol() const;
    void setCursorFromLineCol(size_t line, size_t col, float maxWidth);
};

} // namespace ui

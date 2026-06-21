#pragma once

#include <string>
#include <memory>

#include "Widget.hpp"
#include "core/Types.hpp"
#include "res/Theme.hpp"

namespace ui {

class Font;
class ContextMenu;

class Label : public Widget {
public:
    explicit Label(const std::string& text = "");
    ~Label() override;

    void setText(const std::string& text);
    const std::string& text() const { return m_text; }

    void setTextAlign(TextAlign align) { m_align = align; }
    void setColor(const Color& color) { m_color = color; m_useCustomColor = true; m_useColorRole = false; }
    void setColorRole(ColorRole role) { m_colorRole = role; m_useColorRole = true; m_useCustomColor = false; }
    void setFontSize(float size) { m_fontSize = size; }
    void setFont(class Font* font) { m_font = font; }

    // Selection (read-only — copy only)
    bool hasSelection() const;
    std::string selectedText() const;
    void selectAll();
    void clearSelection();
    void copy();

    const char* typeName() const override { return "Label"; }
    Rect screenRect() const;

    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

    bool onMouseDown(MouseEvent& event) override;
    bool onMouseUp(MouseEvent& event) override;
    bool onMouseMove(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;

private:
    std::string m_text;
    TextAlign m_align = TextAlign::Left;
    Color m_color{1, 1, 1, 1};
    ColorRole m_colorRole = ColorRole::Text;
    bool m_useCustomColor = false;
    bool m_useColorRole = false;
    float m_fontSize = 14.0f;
    class Font* m_font = nullptr;

    // Selection state
    size_t m_selStart = 0, m_selEnd = 0;
    bool m_dragging = false;

    std::unique_ptr<ContextMenu> m_contextMenu;

    size_t posAtX(float x) const;
    void showContextMenu(float x, float y);
};

} // namespace ui

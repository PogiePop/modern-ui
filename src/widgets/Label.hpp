#pragma once

#include <string>

#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class Label : public Widget {
public:
    explicit Label(const std::string& text = "");

    void setText(const std::string& text);
    const std::string& text() const { return m_text; }

    void setTextAlign(TextAlign align) { m_align = align; }
    void setColor(const Color& color) { m_color = color; m_useCustomColor = true; }
    void setFontSize(float size) { m_fontSize = size; }
    void setFont(class Font* font) { m_font = font; }

    const char* typeName() const override { return "Label"; }
    Rect screenRect() const;

    // Overrides
    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

private:
    std::string m_text;
    TextAlign m_align = TextAlign::Left;
    Color m_color{1, 1, 1, 1};
    bool m_useCustomColor = false;
    float m_fontSize = 14.0f;
    class Font* m_font = nullptr;
};

} // namespace ui

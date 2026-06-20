#pragma once

#include <functional>
#include <string>

#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class Button : public Widget {
public:
    explicit Button(const std::string& label = "");

    void setLabel(const std::string& text);
    const std::string& label() const { return m_label; }

    void setOnClick(std::function<void()> callback);
    void click();

    // Visual customization
    void setFont(class Font* f) { m_font = f; }
    void setCornerRadius(float r) { m_cornerRadius = r; }
    void setGradient(Color top, Color bottom) { m_gradTop = top; m_gradBottom = bottom; m_useGradient = true; }
    void setSolidColor(Color c) { m_solidColor = c; m_useGradient = false; }
    void setHoverColor(Color c) { m_hoverColor = c; m_useHoverColor = true; }
    void setPressColor(Color c) { m_pressColor = c; m_usePressColor = true; }

    const char* typeName() const override { return "Button"; }

    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

    bool onMouseDown(MouseEvent& event) override;
    bool onMouseUp(MouseEvent& event) override;
    bool onMouseEnter(MouseEvent& event) override;
    bool onMouseLeave(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;

    void onFocusGained() override;
    void onFocusLost() override;

private:
    std::string m_label;
    std::function<void()> m_onClick;

    enum State { Normal, Hovered, Pressed };
    State m_state = Normal;
    bool m_focusRing = false;

    EdgeInsets m_padding{12, 16, 12, 16};
    float m_cornerRadius = 8;

    Color m_gradTop{0.2f, 0.4f, 0.9f, 1};
    Color m_gradBottom{0.6f, 0.2f, 0.9f, 1};
    Color m_solidColor{0.2f, 0.5f, 0.9f, 1};
    Color m_hoverColor{0.5f, 0.2f, 0.8f, 1};
    Color m_pressColor{0.2f, 0.7f, 1.0f, 1};
    bool m_useGradient = false;
    bool m_useHoverColor = false;
    bool m_usePressColor = false;
    class Font* m_font = nullptr;

    void getCurrentColors(Color& top, Color& bottom) const;
};

} // namespace ui

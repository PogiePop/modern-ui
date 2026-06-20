#pragma once

#include <functional>
#include <string>

#include "Widget.hpp"

namespace ui {

class Checkbox : public Widget {
public:
    explicit Checkbox(const std::string& label = "");

    void setChecked(bool checked);
    bool checked() const { return m_checked; }

    void setLabel(const std::string& label);
    const std::string& label() const { return m_label; }

    void setOnCheckedChanged(std::function<void(bool)> callback);
    void setFont(class Font* font) { m_font = font; }

    const char* typeName() const override { return "Checkbox"; }

    // Overrides
    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

    bool onMouseDown(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;
    bool onMouseEnter(MouseEvent& event) override;
    bool onMouseLeave(MouseEvent& event) override;

    void onFocusGained() override;
    void onFocusLost() override;

private:
    std::string m_label;
    bool m_checked = false;
    std::function<void(bool)> m_onCheckedChanged;
    bool m_hovered = false;
    bool m_focusRing = false;
    class Font* m_font = nullptr;

    static constexpr float BOX_SIZE = 18.0f;
    static constexpr float GAP = 6.0f;
};

} // namespace ui

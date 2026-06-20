#pragma once

#include <functional>

#include "Widget.hpp"

namespace ui {

class Slider : public Widget {
public:
    Slider(float min = 0, float max = 100, float value = 0);

    void setRange(float min, float max);
    void setValue(float val);
    float value() const { return m_value; }
    void setStep(float step) { m_step = step; }

    void setOnValueChanged(std::function<void(float)> cb);

    const char* typeName() const override { return "Slider"; }

    // Overrides
    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

    bool onMouseDown(MouseEvent& event) override;
    bool onMouseMove(MouseEvent& event) override;
    bool onMouseUp(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;
    bool onMouseEnter(MouseEvent& event) override;
    bool onMouseLeave(MouseEvent& event) override;

    void onFocusGained() override;
    void onFocusLost() override;

private:
    float m_min, m_max, m_value, m_step = 0;
    std::function<void(float)> m_onValueChanged;
    bool m_dragging = false;
    bool m_hovered = false;
    bool m_focusRing = false;

    static constexpr float TRACK_HEIGHT = 6.0f;
    static constexpr float THUMB_SIZE = 16.0f;

    float normalizedValue() const;
    float thumbCenterX() const;
    void setValueFromPos(float x);
};

} // namespace ui

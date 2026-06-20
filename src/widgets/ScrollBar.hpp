#pragma once

#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class ScrollBar : public Widget {
public:
    enum Direction { Vertical, Horizontal };

    explicit ScrollBar(Direction dir = Vertical);

    void setRange(float total, float viewport);
    void setValue(float val);
    float value() const { return m_value; }
    float maxValue() const;

    void setOnValueChanged(std::function<void(float)> cb);

    const char* typeName() const override { return "ScrollBar"; }

    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& event) override;
    bool onMouseMove(MouseEvent& event) override;
    bool onMouseUp(MouseEvent& event) override;

private:
    Direction m_dir;
    float m_total = 1;
    float m_viewport = 1;
    float m_value = 0;
    std::function<void(float)> m_onValueChanged;
    bool m_dragging = false;
    float m_dragStartVal = 0;
    float m_dragStartPos = 0;

    static constexpr float MIN_THUMB = 20;
    Rect trackRect() const;
    Rect thumbRect() const;
    float trackLength() const;
    float thumbSize() const;
    float valueFromPos(float pos) const;
};

} // namespace ui

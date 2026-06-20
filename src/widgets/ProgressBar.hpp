#pragma once
#include "Widget.hpp"

namespace ui {

class ProgressBar : public Widget {
public:
    ProgressBar();

    void setValue(float v); // 0..1
    float value() const { return m_value; }
    void setColor(const Color& c) { m_color = c; }
    void setTrackColor(const Color& c) { m_trackColor = c; }

    const char* typeName() const override { return "ProgressBar"; }
    Size measure(const Size&) const override { return {200, 8}; }
    void paint(Painter& p) override;

private:
    float m_value = 0;
    Color m_color{0.2f,0.5f,1,1};
    Color m_trackColor{0.2f,0.2f,0.25f,1};
};

} // namespace ui

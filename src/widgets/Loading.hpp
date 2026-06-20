#pragma once
#include "Widget.hpp"

namespace ui {

class LoadingSpinner : public Widget {
public:
    LoadingSpinner();
    void setColor(const Color& c) { m_color = c; }
    void setSpeed(float s) { m_speed = s; }

    const char* typeName() const override { return "LoadingSpinner"; }
    Size measure(const Size&) const override { return {m_size, m_size}; }
    void paint(Painter& p) override;
    void tickAnimation(float dt) override;

private:
    float m_angle = 0;
    float m_speed = 3.0f;
    float m_size = 24;
    Color m_color{0.3f,0.5f,1,1};
};

} // namespace ui

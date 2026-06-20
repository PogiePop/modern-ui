#pragma once
#include "Widget.hpp"

namespace ui {

// A widget that draws a border around a single child
class Border : public Widget {
public:
    Border();

    void setChild(std::unique_ptr<Widget> child);
    void setStroke(float thickness, const Color& color);
    void setCornerRadius(float r) { m_radius = r; }

    const char* typeName() const override { return "Border"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;

private:
    float m_thickness = 1;
    float m_radius = 0;
    Color m_color{0.5f,0.5f,0.5f,1};
};

} // namespace ui

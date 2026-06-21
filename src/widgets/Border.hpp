#pragma once
#include "Widget.hpp"
#include "res/Theme.hpp"

namespace ui {

class Border : public Widget {
public:
    enum BorderStyle { Solid, Dashed, Dotted };

    Border();

    void setChild(std::unique_ptr<Widget> child);
    void setStroke(float thickness, const Color& color);
    void setStrokeColor(const Color& c) { m_color = c; m_manualColor = true; }
    void setStrokeColorRole(ColorRole r) { m_colorRole = r; m_useColorRole = true; m_manualColor = false; }
    void setCornerRadius(float r) { m_radius = r; }
    void setBorderStyle(BorderStyle s) { m_borderStyle = s; }

    const char* typeName() const override { return "Border"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;

private:
    float m_thickness = 1;
    float m_radius = 0;
    Color m_color{0.5f,0.5f,0.5f,1};
    ColorRole m_colorRole = ColorRole::Border;
    bool m_useColorRole = false, m_manualColor = false;
    BorderStyle m_borderStyle = Solid;
};

} // namespace ui

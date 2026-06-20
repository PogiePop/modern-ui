#pragma once
#include "Widget.hpp"

namespace ui {

// Predefined spacing sizes
enum class SpaceSize {
    XS = 2, SM = 4, MD = 8, LG = 16, XL = 24, XXL = 32
};

// Flexible space that expands to fill remaining room (like SwiftUI Spacer)
class Spacer : public Widget {
public:
    explicit Spacer(SpaceSize min = SpaceSize::XS);
    void setMinSpace(SpaceSize s) { m_min = (float)s; m_minWidth = m_min; m_minHeight = m_min; }

    const char* typeName() const override { return "Spacer"; }
    Size measure(const Size&) const override { return {m_min, m_min}; }
    void paint(Painter&) override {}
private:
    float m_min = 2;
};

// Divider styles
enum class DividerStyle { Solid, Dashed, Dotted };

// Horizontal or vertical separator line
class Divider : public Widget {
public:
    enum Direction { Horizontal, Vertical };

    explicit Divider(Direction dir = Horizontal, DividerStyle style = DividerStyle::Solid,
                     float thickness = 1, const Color& color = Color{0.3f,0.3f,0.4f,1});

    void setStyle(DividerStyle s) { m_style = s; }
    void setColor(const Color& c) { m_color = c; }

    const char* typeName() const override { return "Divider"; }
    Size measure(const Size&) const override {
        return m_dir == Horizontal ? Size{50, m_thickness} : Size{m_thickness, 50};
    }
    void paint(Painter& p) override;

private:
    Direction m_dir;
    DividerStyle m_style;
    float m_thickness;
    Color m_color;
};

// Fixed spacing (shortcut)
inline std::unique_ptr<Spacer> space(SpaceSize s = SpaceSize::MD) { return std::make_unique<Spacer>(s); }
inline std::unique_ptr<Divider> divider(Divider::Direction d = Divider::Horizontal,
    DividerStyle s = DividerStyle::Solid) { return std::make_unique<Divider>(d, s); }

} // namespace ui

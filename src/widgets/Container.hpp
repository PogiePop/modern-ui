#pragma once
#include <optional>
#include <vector>
#include "Widget.hpp"
#include "Layout.hpp"

namespace ui {

// EUI-NEO: Container = Row/Column/Stack layout. ScrollArea handles scrolling.
class Container : public Widget {
public:
    explicit Container(LayoutDirection dir = LayoutDirection::Vertical);

    void setDirection(LayoutDirection dir);
    LayoutDirection direction() const { return m_direction; }

    void setPadding(const EdgeInsets& p) { m_padding = p; m_basePadding = p; }
    EdgeInsets padding() const { return m_padding; }
    void setSpacing(float s) { m_spacing = s; m_baseSpacing = s; }
    float spacing() const { return m_spacing; }
    void setResponsive(bool r) { m_responsive = r; }

    void setBackgroundColor(const Color& color);
    void clearBackgroundColor();
    bool hasBackgroundColor() const { return m_bgColor.has_value(); }
    Color backgroundColor() const;

    void setChildSizing(size_t index, Sizing horz, Sizing vert);

    enum class Align { Start, Center, End, Stretch, SpaceBetween, SpaceAround, SpaceEvenly };
    void setAlign(Align a) { m_align = a; }

    const char* typeName() const override { return "Container"; }

    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;
    Widget* hitTest(Point localPoint) override;
    bool onScrollWheel(float dx, float dy) override;

    struct ChildLayoutInfo { Sizing horz = Sizing::intrinsic(); Sizing vert = Sizing::intrinsic(); };

private:
    LayoutDirection m_direction;
    EdgeInsets m_padding{4,4,4,4}, m_basePadding{4,4,4,4};
    float m_spacing = 4, m_baseSpacing = 4;
    bool m_responsive = false;
    std::optional<Color> m_bgColor;
    std::vector<ChildLayoutInfo> m_childLayout;
    Align m_align = Align::Start;
};

} // namespace ui

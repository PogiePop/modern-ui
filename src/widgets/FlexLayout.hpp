#pragma once
#include "Container.hpp"
#include "Layout.hpp"

namespace ui {

// Enhanced flex-like sizing
enum class FlexWrap { NoWrap, Wrap };
enum class FlexAlign { Start, Center, End, Stretch };

struct FlexItem {
    float grow = 0;      // flex-grow
    float shrink = 1;    // flex-shrink
    float basis = -1;    // -1 = auto (use intrinsic size)
    FlexAlign alignSelf = FlexAlign::Stretch;
};

// Flex container: distributes space using flex-grow/shrink
class FlexLayout : public Container {
public:
    explicit FlexLayout(LayoutDirection dir = LayoutDirection::Horizontal);

    void setWrap(FlexWrap w) { m_wrap = w; }
    void setAlign(FlexAlign a) { m_align = a; }
    void setItemFlex(size_t i, float grow, float shrink, float basis = -1);

    const char* typeName() const override { return "FlexLayout"; }
    Size measure(const Size& a) const override;
    void layout() override;

private:
    FlexWrap m_wrap = FlexWrap::NoWrap;
    FlexAlign m_align = FlexAlign::Start;
    std::vector<FlexItem> m_items;
};

// Responsive breakpoints
enum class ResponsiveSize { XS, SM, MD, LG, XL };

inline ResponsiveSize responsiveFromWidth(float w) {
    if (w < 576) return ResponsiveSize::XS;
    if (w < 768) return ResponsiveSize::SM;
    if (w < 992) return ResponsiveSize::MD;
    if (w < 1200) return ResponsiveSize::LG;
    return ResponsiveSize::XL;
}

// Responsive container: changes direction based on width
class ResponsiveLayout : public Container {
public:
    ResponsiveLayout();
    void setBreakpoint(ResponsiveSize bp, LayoutDirection dir, float spacing, EdgeInsets pad);
    const char* typeName() const override { return "ResponsiveLayout"; }
    void layout() override;
private:
    struct Breakpoint { LayoutDirection dir; float spacing; EdgeInsets pad; };
    Breakpoint m_bps[5];
};

} // namespace ui

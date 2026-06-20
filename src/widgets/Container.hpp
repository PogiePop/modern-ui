#pragma once

#include <optional>
#include <vector>

#include "Widget.hpp"
#include "Layout.hpp"

namespace ui {

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

    // Per-child sizing (index-based)
    void setChildSizing(size_t index, Sizing horz, Sizing vert);

    // Alignment
    enum class Align { Start, Center, End, Stretch, SpaceBetween, SpaceAround, SpaceEvenly };
    void setAlign(Align a) { m_align = a; }
    Align align() const { return m_align; }

    const char* typeName() const override { return "Container"; }
    float scrollOffsetX() const override { return m_scrollable ? m_scrollX : 0; }
    float scrollOffsetY() const override { return m_scrollable ? m_scrollY : 0; }

    // Scrolling
    void setScrollable(bool scrollable) { m_scrollable = scrollable; }
    bool scrollable() const { return m_scrollable; }
    void setAutoScroll(bool on) { m_autoScroll = on; }
    void setScrollX(float x) { m_scrollX = x; }
    void setScrollY(float y) { m_scrollY = y; }
    float scrollX() const { return m_scrollX; }
    float scrollY() const { return m_scrollY; }
    float maxScrollX() const;
    float maxScrollY() const;
    Size contentSize() const { return m_contentSize; }
    bool hasOverflow() const;

    // Overrides
    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;
    Widget* hitTest(Point localPoint) override;
    bool onScrollWheel(float dx, float dy) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseUp(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;

    struct ChildLayoutInfo {
        Sizing horz = Sizing::intrinsic();
        Sizing vert = Sizing::intrinsic();
    };

private:
    LayoutDirection m_direction;
    EdgeInsets m_padding{4, 4, 4, 4};
    EdgeInsets m_basePadding{4, 4, 4, 4};
    float m_spacing = 4;
    float m_baseSpacing = 4;
    bool m_responsive = false;
    std::optional<Color> m_bgColor;
    std::vector<ChildLayoutInfo> m_childLayout;
    bool m_scrollable = false;
    float m_scrollX = 0, m_scrollY = 0;
    bool m_scrollBarDragging = false;
    float m_sbDragScreenY = 0, m_sbDragVal = 0, m_sbTrackScreenY = 0, m_sbTrackLen = 0, m_sbThumbLen = 0, m_sbMaxScroll = 0;
    bool m_autoScroll = false; // off by default, enable on root only
    mutable Size m_contentSize;
    Align m_align = Align::Start;

    float measureAxis(Size available) const;
    float measureCrossAxis(Size available, float mainAxisSize) const;
};

} // namespace ui

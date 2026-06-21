#pragma once
#include "Widget.hpp"

namespace ui {

class ScrollBar;

// EUI-NEO style: wraps content, shares scroll state, clips to viewport
// Usage: scroll->setContent(child); content renders at natural size, scroll offsets in paint
class ScrollArea : public Widget {
public:
    ScrollArea();

    // Builder-style API (EUI-NEO pattern)
    ScrollArea& content(std::unique_ptr<Widget> w);
    ScrollArea& scrollbarWidth(float w) { m_barW = w; return *this; }
    ScrollArea& step(float s) { m_step = s; return *this; }
    ScrollArea& gap(float g) { m_gap = g; return *this; }

    // Access
    Widget* contentWidget() const { return m_content; }
    float scrollX() const { return m_scrollX; }
    float scrollY() const { return m_scrollY; }

    // EUI-NEO: shared scroll state
    void setScrollX(float v);
    void setScrollY(float v);
    float maxScrollX() const;
    float maxScrollY() const;

    // Scroll offset propagates to children's globalPosition / screenRect
    float scrollOffsetX() const override { return m_scrollX; }
    float scrollOffsetY() const override { return m_scrollY; }

    const char* typeName() const override { return "ScrollArea"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;
    bool onMouseUp(MouseEvent& e) override;
    bool onScrollWheel(float dx, float dy) override;
    Widget* hitTest(Point lp) override;

private:
    Widget* m_content = nullptr;
    ScrollBar* m_vBar = nullptr;
    ScrollBar* m_hBar = nullptr;
    float m_scrollX = 0, m_scrollY = 0;
    float m_barW = 8, m_step = 40, m_gap = 4;
    Size m_contentSize;
    // EUI-NEO: shared scroll state between viewport, content, and scrollbars
    void syncBars();
    void ensureBars();
};

} // namespace ui

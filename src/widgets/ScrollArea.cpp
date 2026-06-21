#include "ScrollArea.hpp"
#include "ScrollBar.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include <algorithm>
#include <cmath>

namespace ui {

ScrollArea::ScrollArea() { m_focusable = false; }

ScrollArea& ScrollArea::content(std::unique_ptr<Widget> w) {
    removeAllChildren();
    m_content = w.get();
    if (m_content) addChild(std::move(w));
    markLayoutDirty();
    return *this;
}

void ScrollArea::setScrollX(float v) { m_scrollX = std::clamp(v, 0.0f, maxScrollX()); syncBars(); }
void ScrollArea::setScrollY(float v) { m_scrollY = std::clamp(v, 0.0f, maxScrollY()); syncBars(); }

float ScrollArea::maxScrollX() const { return std::max(0.0f, m_contentSize.width - m_bounds.width); }
float ScrollArea::maxScrollY() const { return std::max(0.0f, m_contentSize.height - m_bounds.height + 30); }

Size ScrollArea::measure(const Size& a) const {
    if (m_content) return m_content->measure(a);
    return {a.width, a.height};
}

void ScrollArea::layout() {
    if (!m_content) return;
    Size ms = m_content->measure({m_bounds.width, std::numeric_limits<float>::max()});
    m_content->setBounds({0, 0, std::max(ms.width, m_bounds.width), ms.height});
    m_content->layout();
    m_contentSize = {m_content->bounds().width, m_content->bounds().height};
    ensureBars();
    m_scrollX = std::clamp(m_scrollX, 0.0f, maxScrollX());
    m_scrollY = std::clamp(m_scrollY, 0.0f, maxScrollY());
}

void ScrollArea::ensureBars() {
    bool needV = maxScrollY() > 0;

    // Only right-side vertical bar — no horizontal bar
    if (m_hBar) { removeChild(m_hBar); m_hBar = nullptr; }

    if (needV && !m_vBar) {
        auto bar = std::make_unique<ScrollBar>(ScrollBar::Vertical);
        m_vBar = bar.get();
        m_vBar->setMinSize(m_barW, 0);
        addChild(std::move(bar));
    } else if (!needV && m_vBar) { removeChild(m_vBar); m_vBar = nullptr; }

    if (m_vBar) m_vBar->setBounds({m_bounds.width - m_barW - m_gap, 0, m_barW, m_bounds.height});

    syncBars();
}

void ScrollArea::syncBars() {
    if (m_vBar) { m_vBar->setRange(m_contentSize.height + 30, m_bounds.height); m_vBar->setValue(m_scrollY); }
}

// paint with viewport clip — scroll offset handled by globalPosition
void ScrollArea::paint(Painter& p) {
    Rect r = screenRect();
    Rect vp{r.x, r.y, m_bounds.width - (m_vBar ? m_barW + m_gap : 0), m_bounds.height};
    p.pushClip(vp);

    if (m_content) {
        // Content and children use screenRect() which includes scroll offset
        // via scrollOffsetX/Y override → no manual position adjustment needed
        m_content->paint(p);
    }
    p.popClip();

    if (m_vBar && m_vBar->visible()) m_vBar->paint(p);
    if (m_hBar && m_hBar->visible()) m_hBar->paint(p);
}

bool ScrollArea::onMouseDown(MouseEvent& e) {
    if (m_vBar && m_vBar->visible()) { Point bl{e.localPos.x-m_vBar->bounds().x, e.localPos.y-m_vBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; if (m_vBar->hitTest(bl)) return m_vBar->onMouseDown(ce); }
    if (m_hBar && m_hBar->visible()) { Point bl{e.localPos.x-m_hBar->bounds().x, e.localPos.y-m_hBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; if (m_hBar->hitTest(bl)) return m_hBar->onMouseDown(ce); }
    if (m_content && m_content->visible()) { MouseEvent ce=e; ce.localPos.x+=m_scrollX; ce.localPos.y+=m_scrollY; return m_content->onMouseDown(ce); }
    return false;
}

bool ScrollArea::onMouseMove(MouseEvent& e) {
    if (m_vBar && m_vBar->visible()) { Point bl{e.localPos.x-m_vBar->bounds().x, e.localPos.y-m_vBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; if (m_vBar->hitTest(bl)) { m_vBar->onMouseMove(ce); return true; } }
    if (m_hBar && m_hBar->visible()) { Point bl{e.localPos.x-m_hBar->bounds().x, e.localPos.y-m_hBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; if (m_hBar->hitTest(bl)) { m_hBar->onMouseMove(ce); return true; } }
    return false;
}

bool ScrollArea::onMouseUp(MouseEvent& e) {
    if (m_vBar) { Point bl{e.localPos.x-m_vBar->bounds().x, e.localPos.y-m_vBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; m_vBar->onMouseUp(ce); }
    if (m_hBar) { Point bl{e.localPos.x-m_hBar->bounds().x, e.localPos.y-m_hBar->bounds().y}; MouseEvent ce=e; ce.localPos=bl; m_hBar->onMouseUp(ce); }
    return false;
}

bool ScrollArea::onScrollWheel(float dx, float dy) {
    if (maxScrollY() > 0) { setScrollY(m_scrollY - dy * m_step); return true; }
    if (maxScrollX() > 0) { setScrollX(m_scrollX - dx * m_step); return true; }
    return false;
}

Widget* ScrollArea::hitTest(Point lp) {
    if (!m_visible) return nullptr;
    if (lp.x < 0 || lp.y < 0 || lp.x >= m_bounds.width || lp.y >= m_bounds.height) return nullptr;
    if (m_vBar && m_vBar->visible()) { Point bl{lp.x-m_vBar->bounds().x, lp.y-m_vBar->bounds().y}; Widget* h=m_vBar->hitTest(bl); if (h) return h; }
    if (m_hBar && m_hBar->visible()) { Point bl{lp.x-m_hBar->bounds().x, lp.y-m_hBar->bounds().y}; Widget* h=m_hBar->hitTest(bl); if (h) return h; }
    if (m_content && m_content->visible()) { Point cl{lp.x+m_scrollX, lp.y+m_scrollY}; return m_content->hitTest(cl); }
    return this;
}

} // namespace ui

#include "FlexLayout.hpp"
#include <algorithm>
#include <cmath>

namespace ui {

FlexLayout::FlexLayout(LayoutDirection dir) : Container(dir) {}

void FlexLayout::setItemFlex(size_t i, float grow, float shrink, float basis) {
    if (i >= m_items.size()) m_items.resize(i + 1);
    m_items[i] = {grow, shrink, basis, FlexAlign::Stretch};
}

Size FlexLayout::measure(const Size& a) const {
    float totalW = padding().horizontal(), totalH = padding().vertical();
    if (direction() == LayoutDirection::Horizontal) {
        for (size_t i = 0; i < m_children.size(); ++i) {
            Size cs = m_children[i]->measure(a);
            totalW += cs.width + (i > 0 ? spacing() : 0);
            if (cs.height > totalH) totalH = cs.height;
        }
    } else {
        for (size_t i = 0; i < m_children.size(); ++i) {
            Size cs = m_children[i]->measure(a);
            totalH += cs.height + (i > 0 ? spacing() : 0);
            if (cs.width > totalW) totalW = cs.width;
        }
    }
    return {std::max(totalW, m_minWidth), std::max(totalH, m_minHeight)};
}

void FlexLayout::layout() {
    if (m_children.empty()) return;
    EdgeInsets p = padding();
    float contentW = m_bounds.width - p.horizontal();
    float contentH = m_bounds.height - p.vertical();
    float x = p.left, y = p.top;
    bool horiz = direction() == LayoutDirection::Horizontal;
    float totalBasis = 0, totalGrow = 0;
    std::vector<float> sizes(m_children.size(), 0);

    for (size_t i = 0; i < m_children.size(); ++i) {
        Size ms = m_children[i]->measure({contentW, contentH});
        float basis = (i < m_items.size() && m_items[i].basis >= 0) ? m_items[i].basis :
                      (horiz ? ms.width : ms.height);
        sizes[i] = basis;
        totalBasis += basis + (i > 0 ? spacing() : 0);
        if (i < m_items.size()) totalGrow += m_items[i].grow;
    }

    float remaining = (horiz ? contentW : contentH) - totalBasis;
    if (remaining > 0 && totalGrow > 0) {
        for (size_t i = 0; i < m_children.size(); ++i)
            if (i < m_items.size() && m_items[i].grow > 0)
                sizes[i] += remaining * m_items[i].grow / totalGrow;
    }

    for (size_t i = 0; i < m_children.size(); ++i) {
        float childW = horiz ? sizes[i] : contentW;
        float childH = horiz ? contentH : sizes[i];
        if (childW < m_children[i]->minWidth()) childW = m_children[i]->minWidth();
        if (childH < m_children[i]->minHeight()) childH = m_children[i]->minHeight();
        m_children[i]->setBounds({x, y, childW, childH});
        if (horiz) x += childW + spacing(); else y += childH + spacing();
        m_children[i]->layout();
    }
}

// Responsive
ResponsiveLayout::ResponsiveLayout() : Container(LayoutDirection::Horizontal) {
    for (int i = 0; i < 5; ++i)
        m_bps[i] = {LayoutDirection::Vertical, 8, {12,12,12,12}};
}

void ResponsiveLayout::setBreakpoint(ResponsiveSize bp, LayoutDirection dir, float sp, EdgeInsets pad) {
    m_bps[(int)bp] = {dir, sp, pad};
}

void ResponsiveLayout::layout() {
    ResponsiveSize rs = responsiveFromWidth(m_bounds.width);
    auto& bp = m_bps[(int)rs];
    setDirection(bp.dir);
    setSpacing(bp.spacing);
    setPadding(bp.pad);
    Container::layout();
}

} // namespace ui

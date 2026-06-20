#include "GridLayout.hpp"
#include "core/Painter.hpp"
#include <algorithm>

namespace ui {

GridLayout::GridLayout() { m_focusable = false; m_colWidths.resize(m_cols, 100); }

void GridLayout::setColumns(int count, float* widths) {
    m_cols = count; m_colWidths.resize(count);
    if (widths) for (int i = 0; i < count; ++i) m_colWidths[i] = widths[i];
    else { float eq = 100.0f; for (int i = 0; i < count; ++i) m_colWidths[i] = eq; }
}

void GridLayout::addCell(int row, int col, std::unique_ptr<Widget> child, int rowSpan, int colSpan) {
    m_cells.push_back({row, col, rowSpan, colSpan});
    addChild(std::move(child));
}

float GridLayout::colWidth(int c) const {
    if (c < 0 || c >= m_cols) return 0;
    float total = 0; for (float w : m_colWidths) total += w;
    float unit = m_bounds.width > 0 ? (m_bounds.width - (m_cols-1)*m_hGap) / total : 100;
    return m_colWidths[c] * unit;
}

Size GridLayout::measure(const Size& a) const {
    (void)a;
    int maxRow = 0; float maxH = 0;
    for (size_t i = 0; i < m_cells.size(); ++i) {
        int r = m_cells[i].row + m_cells[i].rowSpan;
        if (r > maxRow) maxRow = r;
    }
    for (size_t i = 0; i < m_children.size(); ++i)
        maxH = std::max(maxH, m_children[i]->measure({100, 30}).height);
    float w = 0; for (float cw : m_colWidths) w += cw;
    return {w + (m_cols-1)*m_hGap, maxRow * (maxH + m_vGap)};
}

void GridLayout::layout() {
    for (size_t i = 0; i < m_cells.size() && i < m_children.size(); ++i) {
        auto& cell = m_cells[i];
        float x = 0; for (int c = 0; c < cell.col; ++c) x += colWidth(c) + m_hGap;
        float w = 0; for (int c = cell.col; c < cell.col + cell.colSpan; ++c) w += colWidth(c) + (c > cell.col ? m_hGap : 0);
        float y = cell.row * (m_children[i]->measure({w, 30}).height + m_vGap);
        float h = m_children[i]->measure({w, 30}).height;
        m_children[i]->setBounds({x, y, w, h});
        m_children[i]->layout();
    }
}

void GridLayout::paint(Painter& p) {
    for (auto& c : m_children) if (c->visible()) c->paint(p);
}

} // namespace ui

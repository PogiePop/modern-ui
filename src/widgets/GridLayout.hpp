#pragma once
#include <vector>
#include "Widget.hpp"

namespace ui {

// Simple grid layout: defines columns, rows auto-size, supports span
class GridLayout : public Widget {
public:
    GridLayout();

    void setColumns(int count, float* widths = nullptr); // nullptr = equal
    void addCell(int row, int col, std::unique_ptr<Widget> child, int rowSpan = 1, int colSpan = 1);
    void setSpacing(float h, float v) { m_hGap = h; m_vGap = v; }

    const char* typeName() const override { return "GridLayout"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;

private:
    int m_cols = 2;
    std::vector<float> m_colWidths;
    float m_hGap = 8, m_vGap = 8;
    struct Cell { int row, col, rowSpan, colSpan; };
    std::vector<Cell> m_cells;
    float colWidth(int c) const;
};

} // namespace ui

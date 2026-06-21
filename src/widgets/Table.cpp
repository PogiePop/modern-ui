#include "Table.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"

namespace ui {
Table::Table() { m_focusable = false; }

void Table::setColumns(const std::vector<std::string>& headers, const std::vector<float>& widths) {
    m_headers = headers;
    m_widths = widths.empty() ? std::vector<float>(headers.size(), 100.0f) : widths;
}

void Table::addRow(const std::vector<std::string>& cells) { m_rows.push_back(cells); }
void Table::clearRows() { m_rows.clear(); }

Size Table::measure(const Size& a) const {
    float w = 0; for (float cw : m_widths) w += cw;
    return {w, m_headerH + m_rows.size() * m_rowH};
}

void Table::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    float y = r.y;
    float x = r.x;
    // Header
    p.drawRect({x, y, m_bounds.width, m_headerH}, Color::fromHex(0xFF2A2A3E));
    for (size_t i = 0; i < m_headers.size() && i < m_widths.size(); ++i) {
        p.drawText({x+4, y+4, m_widths[i]-8, lh}, m_headers[i], Color{0.9f,0.9f,0.9f,1}, TextAlign::Left);
        x += m_widths[i];
    }
    y += m_headerH;
    // Rows
    for (size_t row = 0; row < m_rows.size(); ++row) {
        x = r.x;
        Color rowBg = (row % 2 == 0) ? Color::fromHex(0xFF1E1E2E) : Color::fromHex(0xFF222233);
        p.drawRect({x, y, m_bounds.width, m_rowH}, rowBg);
        for (size_t col = 0; col < m_rows[row].size() && col < m_widths.size(); ++col) {
            p.drawText({x+4, y+4, m_widths[col]-8, lh}, m_rows[row][col], Color{0.8f,0.8f,0.8f,1}, TextAlign::Left);
            x += m_widths[col];
        }
        y += m_rowH;
    }
}
} // namespace ui

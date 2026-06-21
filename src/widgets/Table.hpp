#pragma once
#include <vector>
#include <string>
#include "Widget.hpp"

namespace ui {

class Table : public Widget {
public:
    Table();

    void setColumns(const std::vector<std::string>& headers, const std::vector<float>& widths = {});
    void addRow(const std::vector<std::string>& cells);
    void clearRows();
    void setFont(class Font* f) { m_font = f; }
    void setRowHeight(float h) { m_rowH = h; }
    void setHeaderHeight(float h) { m_headerH = h; }

    const char* typeName() const override { return "Table"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;

private:
    std::vector<std::string> m_headers;
    std::vector<float> m_widths;
    std::vector<std::vector<std::string>> m_rows;
    class Font* m_font = nullptr;
    float m_rowH = 28, m_headerH = 32;
};

} // namespace ui

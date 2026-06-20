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

    const char* typeName() const override { return "Table"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;

private:
    std::vector<std::string> m_headers;
    std::vector<float> m_widths;
    std::vector<std::vector<std::string>> m_rows;
    class Font* m_font = nullptr;
    static constexpr float ROW_H = 28, HEADER_H = 32;
};

} // namespace ui

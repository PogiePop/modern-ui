#pragma once
#include <functional>
#include <string>
#include "Widget.hpp"

namespace ui {

class Pagination : public Widget {
public:
    Pagination();

    void setTotalPages(int n) { m_total = n; rebuild(); }
    void setCurrentPage(int n);
    int currentPage() const { return m_current; }
    void setOnPageChanged(std::function<void(int)> cb) { m_onChanged = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }
    void setRounded(bool r) { m_rounded = r; }

    const char* typeName() const override { return "Pagination"; }
    Size measure(const Size&) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    Widget* hitTest(Point lp) override;

private:
    int m_total = 5, m_current = 1;
    std::function<void(int)> m_onChanged;
    class Font* m_font = nullptr;
    bool m_rounded = false;
    void rebuild();
};

} // namespace ui

#include "Pagination.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"

namespace ui {
Pagination::Pagination() { m_focusable = false; rebuild(); }

void Pagination::setCurrentPage(int n) {
    if (n >= 1 && n <= m_total) { m_current = n; rebuild(); if (m_onChanged) m_onChanged(n); }
}

void Pagination::rebuild() { /* page count changed, will reflect in next paint */ }

Size Pagination::measure(const Size&) const {
    float bw = 32, gap = 4;
    return {(bw+gap)*m_total + 60, 32};
}

void Pagination::paint(Painter& p) {
    Rect r = screenRect();
    float bw = 32, gap = 4, y = r.y + 4, h = 24;
    auto drawBtn = [&](float x, const std::string& label, bool active) {
        Rect br{x, y, bw, h};
        Color bg = active ? Color{0.3f,0.5f,1,1} : Color{0.15f,0.15f,0.2f,1};
        Color txt = active ? Color{1,1,1,1} : Color{0.7f,0.7f,0.7f,1};
        if (m_theme) { bg = active ? m_theme->color(ColorRole::Primary) : m_theme->color(ColorRole::Surface); txt = active ? Color{1,1,1,1} : m_theme->color(ColorRole::TextSecondary); }
        float rad = m_rounded ? h*0.5f : 4.0f;
        p.drawRoundedRect(br, bg, rad);
        p.drawText(br, label, txt, TextAlign::Center);
    };
    float x = r.x;
    drawBtn(x, "<", false); x += bw + gap;
    for (int i = 1; i <= m_total; ++i) { drawBtn(x, std::to_string(i), i == m_current); x += bw + gap; }
    drawBtn(x, ">", false);
}

bool Pagination::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    float bw = 32, gap = 4, relX = e.localPos.x;
    int idx = (int)((relX - (bw+gap)) / (bw+gap)) + 1;
    if (idx >= 1 && idx <= m_total) setCurrentPage(idx);
    else if (relX < bw) setCurrentPage(m_current - 1);
    else setCurrentPage(m_current + 1);
    return true;
}

Widget* Pagination::hitTest(Point lp) {
    if (!m_visible) return nullptr;
    if (lp.x >= 0 && lp.y >= 0 && lp.x < m_bounds.width && lp.y < m_bounds.height) return this;
    return nullptr;
}
} // namespace ui

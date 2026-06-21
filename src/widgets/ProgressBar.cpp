#include "ProgressBar.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"
#include <algorithm>

namespace ui {
ProgressBar::ProgressBar() { m_focusable = false; }
void ProgressBar::setValue(float v) { m_value = std::clamp(v, 0.0f, 1.0f); }

void ProgressBar::paint(Painter& p) {
    Rect r = screenRect();

    // Theme colors — priority: custom color > color role > theme default
    Color trackCol = m_trackColor;
    Color fillCol = m_color;
    if (m_theme) {
        trackCol = m_theme->color(ColorRole::BgTertiary);
        if (m_useColorRole) fillCol = m_theme->color(m_colorRole);
        else if (!m_colorCustom) fillCol = m_theme->color(ColorRole::Primary);
    }

    float rad = r.height * 0.5f;
    p.drawRoundedRect(r, trackCol, rad);
    if (m_value > 0) {
        Rect fill{r.x, r.y, r.width * m_value, r.height};
        p.drawRoundedRect(fill, fillCol, rad);
    }
}
} // namespace ui

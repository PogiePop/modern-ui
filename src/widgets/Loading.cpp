#include "Loading.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"
#include <cmath>

namespace ui {
LoadingSpinner::LoadingSpinner() { m_focusable = false; }

void LoadingSpinner::tickAnimation(float dt) { m_angle += m_speed * dt; }

void LoadingSpinner::paint(Painter& p) {
    Rect r = screenRect();
    float cx = r.x + r.width*0.5f, cy = r.y + r.height*0.5f, rad = r.width*0.4f;
    Color baseColor = m_color;
    if (m_theme && m_useRole) baseColor = m_theme->color(m_colorRole);
    else if (m_theme && !m_custom) baseColor = m_theme->color(ColorRole::Primary);
    for (int i = 0; i < 8; ++i) {
        float a = m_angle + i * 0.785398f;
        float alpha = 0.2f + 0.8f * (i / 7.0f);
        float sx = cx + cosf(a)*rad, sy = cy + sinf(a)*rad;
        Color c{baseColor.r, baseColor.g, baseColor.b, baseColor.a * alpha};
        float dot = r.width * 0.06f;
        p.drawRoundedRect({sx-dot, sy-dot, dot*2, dot*2}, c, dot);
    }
}
} // namespace ui

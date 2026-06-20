#include "Loading.hpp"
#include "core/Painter.hpp"
#include <cmath>

namespace ui {
LoadingSpinner::LoadingSpinner() { m_focusable = false; }

void LoadingSpinner::tickAnimation(float dt) { m_angle += m_speed * dt; }

void LoadingSpinner::paint(Painter& p) {
    Rect r = screenRect();
    float cx = r.x + r.width*0.5f, cy = r.y + r.height*0.5f, rad = r.width*0.4f;
    // Draw arc segments
    for (int i = 0; i < 8; ++i) {
        float a = m_angle + i * 0.785398f; // 45 degrees each
        float alpha = 0.2f + 0.8f * (i / 7.0f); // fade trailing
        float sx = cx + cosf(a)*rad, sy = cy + sinf(a)*rad;
        Color c{m_color.r, m_color.g, m_color.b, m_color.a * alpha};
        p.drawRoundedRect({sx-3, sy-3, 6, 6}, c, 3);
    }
}
} // namespace ui

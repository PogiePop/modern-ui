#include "ProgressBar.hpp"
#include "core/Painter.hpp"
#include <algorithm>

namespace ui {
ProgressBar::ProgressBar() { m_focusable = false; }
void ProgressBar::setValue(float v) { m_value = std::clamp(v, 0.0f, 1.0f); }

void ProgressBar::paint(Painter& p) {
    Rect r = screenRect();
    p.drawRoundedRect(r, m_trackColor, r.height*0.5f);
    if (m_value > 0) {
        Rect fill{r.x, r.y, r.width*m_value, r.height};
        p.drawRoundedRect(fill, m_color, r.height*0.5f);
    }
}
} // namespace ui

#include "Switch.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"

namespace ui {

Switch::Switch() { m_focusable = true; m_animPos = m_checked ? 1 : 0; }
void Switch::setChecked(bool c) { m_checked = c; m_animPos = c ? 1.0f : 0.0f; if (m_onChanged) m_onChanged(c); }

void Switch::paint(Painter& p) {
    Rect r = screenRect();
    float cx = r.x + r.width * 0.5f, cy = r.y + r.height * 0.5f;
    float tw = r.width, th = r.height, knobR = th * 0.4f;

    // Track color from theme
    Color trackCol = m_checked ? Color{0.2f, 0.7f, 0.3f, 1} : Color{0.3f, 0.3f, 0.4f, 1};
    if (m_theme) {
        ColorRole cr = m_useColorRole ? m_colorRole : ColorRole::Primary;
        trackCol = m_checked ? m_theme->color(cr) : m_theme->color(ColorRole::BgTertiary);
    }
    p.drawRoundedRect({r.x, r.y, tw, th}, trackCol, th * 0.5f);

    // Knob
    float kx = r.x + knobR + (tw - knobR * 2) * m_animPos;
    Color knobCol{1, 1, 1, 1};
    if (m_theme) knobCol = m_theme->color(ColorRole::Surface);
    p.drawRoundedRect({kx - knobR, cy - knobR, knobR * 2, knobR * 2}, knobCol, knobR);
    // Small shadow on knob
    p.drawShadow({kx - knobR, cy - knobR, knobR * 2, knobR * 2}, knobR, Color{0, 0, 0, 0.2f}, {0, 1}, 2);
}

bool Switch::onMouseDown(MouseEvent& e) {
    if (e.button == 0) { m_checked = !m_checked; m_animPos = m_checked ? 1.0f : 0.0f; if (m_onChanged) m_onChanged(m_checked); return true; }
    return false;
}
bool Switch::onKeyDown(KeyEvent& e) {
    if (e.type == KeyEvent::Down && (e.key == 32 || e.key == 257)) { setChecked(!m_checked); return true; }
    return false;
}

} // namespace ui

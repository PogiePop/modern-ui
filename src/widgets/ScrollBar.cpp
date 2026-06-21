#include "ScrollBar.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include <algorithm>

namespace ui {

ScrollBar::ScrollBar(Direction dir) : m_dir(dir) { m_focusable = false; }

// VIEWPORT-MODEL: setRange without side effects
void ScrollBar::setRange(float total, float viewport) {
    m_total = total > 0 ? total : 1;
    m_viewport = std::clamp(viewport, 0.0f, m_total);
    if (m_value > maxValue()) m_value = maxValue();
}

// VIEWPORT-MODEL: setValue clamps and fires callback only on change
void ScrollBar::setValue(float val) {
    float clamped = std::clamp(val, 0.0f, maxValue());
    if (clamped != m_value) { m_value = clamped; if (m_onValueChanged) m_onValueChanged(m_value); }
}

float ScrollBar::maxValue() const { return std::max(0.0f, m_total - m_viewport); }
void ScrollBar::setOnValueChanged(std::function<void(float)> cb) { m_onValueChanged = std::move(cb); }

// VIEWPORT-MODEL: track geometry based purely on own bounds
float ScrollBar::trackLength() const { return m_dir == Vertical ? m_bounds.height : m_bounds.width; }

float ScrollBar::thumbSize() const {
    if (m_total <= 0 || m_viewport >= m_total) return trackLength();
    return std::max(MIN_THUMB, trackLength() * (m_viewport / m_total));
}

// VIEWPORT-MODEL: thumb position in local coords, no external offsets
Rect ScrollBar::thumbRect() const {
    float tLen = trackLength(), tSize = thumbSize(), range = maxValue();
    float frac = range > 0 ? m_value / range : 0;
    float offset = (tLen - tSize) * frac;
    if (m_dir == Vertical) return {1, offset, m_bounds.width - 2, tSize};
    return {offset, 1, tSize, m_bounds.height - 2};
}

// VIEWPORT-MODEL: local pos → scroll value
float ScrollBar::valueFromPos(float localPos) const {
    float tLen = trackLength(), tSize = thumbSize(), range = maxValue();
    if (tLen <= tSize) return 0;
    return std::clamp((localPos - tSize*0.5f) / (tLen - tSize) * range, 0.0f, range);
}

Size ScrollBar::measure(const Size&) const { return m_dir == Vertical ? Size{10,100} : Size{100,10}; }

// VIEWPORT-MODEL: paint uses screenRect (fixed viewport position, no container scroll)
// Element-Plus style: thin, rounded, semi-transparent, visible on hover
void ScrollBar::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    // Track — transparent (no track drawn in Element-Plus style)
    // Thumb — rounded pill, semi-transparent
    Rect t = thumbRect();
    Rect st{sx + t.x + 1, sy + t.y + 1, t.width - 2, t.height - 2};
    Color tc = m_dragging ? Color{0.4f, 0.4f, 0.45f, 0.9f} : Color{0.35f, 0.35f, 0.4f, 0.45f};
    float rad = st.width < st.height ? st.width * 0.5f : st.height * 0.5f;
    painter.drawRoundedRect(st, tc, rad);
}

// VIEWPORT-MODEL: e.localPos is already in local coords (App uses screenRect for conversion)
bool ScrollBar::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    Rect thumb = thumbRect();
    if (thumb.contains(e.localPos)) {
        m_dragging = true; m_dragStartVal = m_value;
        m_dragStartPos = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
        return true;
    }
    // Click on track: jump to position
    float pos = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
    setValue(valueFromPos(pos));
    m_dragging = true; m_dragStartVal = m_value; m_dragStartPos = pos;
    return true;
}

// VIEWPORT-MODEL: drag delta in local space → value change → callback → Container updates
bool ScrollBar::onMouseMove(MouseEvent& e) {
    if (!m_dragging) return false;
    float cur = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
    float delta = cur - m_dragStartPos;
    float tLen = trackLength(), tSize = thumbSize(), range = maxValue();
    if (tLen > tSize && range > 0)
        setValue(m_dragStartVal + delta * range / (tLen - tSize));
    return true;
}

bool ScrollBar::onMouseUp(MouseEvent& e) {
    if (e.button == 0) { m_dragging = false; return true; }
    return false;
}

} // namespace ui

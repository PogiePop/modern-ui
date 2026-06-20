#include "ScrollBar.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include <algorithm>

namespace ui {

ScrollBar::ScrollBar(Direction dir) : m_dir(dir) { m_focusable = false; }

void ScrollBar::setRange(float total, float viewport) {
    m_total = total > 0 ? total : 1;
    m_viewport = std::clamp(viewport, 0.0f, m_total);
    if (m_value > maxValue()) m_value = maxValue();
}

void ScrollBar::setValue(float val) {
    m_value = std::clamp(val, 0.0f, maxValue());
    if (m_onValueChanged) m_onValueChanged(m_value);
}

float ScrollBar::maxValue() const { return std::max(0.0f, m_total - m_viewport); }

void ScrollBar::setOnValueChanged(std::function<void(float)> cb) {
    m_onValueChanged = std::move(cb);
}

float ScrollBar::trackLength() const {
    return m_dir == Vertical ? m_bounds.height : m_bounds.width;
}

float ScrollBar::thumbSize() const {
    float ratio = m_viewport / m_total;
    return std::max(MIN_THUMB, trackLength() * ratio);
}

Rect ScrollBar::trackRect() const {
    if (m_dir == Vertical) return {0, 0, m_bounds.width, m_bounds.height};
    return {0, 0, m_bounds.width, m_bounds.height};
}

Rect ScrollBar::thumbRect() const {
    float tLen = trackLength();
    float tSize = thumbSize();
    float range = maxValue();
    float frac = range > 0 ? m_value / range : 0;
    float offset = (tLen - tSize) * frac;

    if (m_dir == Vertical) return {1, offset, m_bounds.width - 2, tSize};
    return {offset, 1, tSize, m_bounds.height - 2};
}

float ScrollBar::valueFromPos(float pos) const {
    float tLen = trackLength();
    float tSize = thumbSize();
    float range = maxValue();
    if (tLen <= tSize) return 0;
    float frac = (pos - tSize * 0.5f) / (tLen - tSize);
    return std::clamp(frac * range, 0.0f, range);
}

Size ScrollBar::measure(const Size&) const {
    if (m_dir == Vertical) return {12, 100};
    return {100, 12};
}

void ScrollBar::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    Rect tr = trackRect();
    painter.drawRect({sx + tr.x, sy + tr.y, tr.width, tr.height}, Color::fromHex(0xFF222222));

    // Thumb
    Rect thumb = thumbRect();
    Rect st{sx + thumb.x, sy + thumb.y, thumb.width, thumb.height};
    Color thumbCol = m_dragging ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF555555);
    painter.drawRect(st, thumbCol);
}

bool ScrollBar::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    Rect thumb = thumbRect();
    if (thumb.contains(e.localPos)) {
        m_dragging = true;
        m_dragStartVal = m_value;
        m_dragStartPos = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
        return true;
    }
    // Jump to position
    setValue(valueFromPos((m_dir == Vertical) ? e.localPos.y : e.localPos.x));
    m_dragging = true;
    m_dragStartVal = m_value;
    m_dragStartPos = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
    return true;
}

bool ScrollBar::onMouseMove(MouseEvent& e) {
    if (!m_dragging) return false;
    float cur = (m_dir == Vertical) ? e.localPos.y : e.localPos.x;
    float delta = cur - m_dragStartPos;
    float tLen = trackLength();
    float tSize = thumbSize();
    float range = maxValue();
    if (tLen > tSize && range > 0) {
        float valDelta = delta * range / (tLen - tSize);
        setValue(m_dragStartVal + valDelta);
    }
    return true;
}

bool ScrollBar::onMouseUp(MouseEvent& e) {
    if (e.button == 0) { m_dragging = false; return true; }
    return false;
}

} // namespace ui

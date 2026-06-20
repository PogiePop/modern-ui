#include "Slider.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"

#include <algorithm>

namespace ui {

Slider::Slider(float min, float max, float value)
    : m_min(min), m_max(max), m_value(value) {
    m_focusable = true;
    m_tabIndex = 0;
}

void Slider::setRange(float min, float max) {
    m_min = min;
    m_max = max;
    if (m_value < m_min) m_value = m_min;
    if (m_value > m_max) m_value = m_max;
}

void Slider::setValue(float val) {
    m_value = std::clamp(val, m_min, m_max);
    if (m_step > 0) {
        m_value = std::round(m_value / m_step) * m_step;
        m_value = std::clamp(m_value, m_min, m_max);
    }
}

void Slider::setOnValueChanged(std::function<void(float)> cb) {
    m_onValueChanged = std::move(cb);
}

float Slider::normalizedValue() const {
    if (m_max <= m_min) return 0;
    return (m_value - m_min) / (m_max - m_min);
}

float Slider::thumbCenterX() const {
    float trackW = m_bounds.width - THUMB_SIZE;
    return THUMB_SIZE * 0.5f + normalizedValue() * trackW;
}

void Slider::setValueFromPos(float x) {
    float trackStart = THUMB_SIZE * 0.5f;
    float trackW = m_bounds.width - THUMB_SIZE;
    float t = (x - trackStart) / trackW;
    t = std::clamp(t, 0.0f, 1.0f);
    float newVal = m_min + t * (m_max - m_min);
    setValue(newVal);
    if (m_onValueChanged) m_onValueChanged(m_value);
}

Size Slider::measure(const Size& available) const {
    (void)available;
    return {200, THUMB_SIZE + 8}; // default width 200
}

void Slider::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    float trackY = sy + (m_bounds.height - TRACK_HEIGHT) * 0.5f;
    float thumbX = sx + thumbCenterX() - THUMB_SIZE * 0.5f;
    float thumbY = sy + (m_bounds.height - THUMB_SIZE) * 0.5f;

    // Track background
    Rect trackBg{sx, trackY, m_bounds.width, TRACK_HEIGHT};
    painter.drawRect(trackBg, Color::fromHex(0xFF444444));

    // Active track (width = thumb center distance from left edge)
    Rect trackActive{sx, trackY, thumbCenterX(), TRACK_HEIGHT};
    painter.drawRect(trackActive, Color::fromHex(0xFF3B82F6));

    // Thumb
    Rect thumbRect{thumbX, thumbY, THUMB_SIZE, THUMB_SIZE};
    Color thumbColor = m_dragging
        ? Color::fromHex(0xFF2563EB)
        : (m_hovered ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF60A5FA));
    painter.drawRect(thumbRect, thumbColor);

    // Focus ring
    if (m_focusRing) {
        painter.drawRectOutline(thumbRect.insetBy(-2, -2),
            Color::fromHex(0xFFFFFFFF), 2);
    }
}

bool Slider::onMouseDown(MouseEvent& event) {
    if (event.button == 0) {
        // Check if click is on thumb
        float thumbX = thumbCenterX() - THUMB_SIZE * 0.5f;
        float thumbY = (m_bounds.height - THUMB_SIZE) * 0.5f;
        Rect thumbRect{thumbX, thumbY, THUMB_SIZE, THUMB_SIZE};

        if (thumbRect.contains(event.localPos)) {
            m_dragging = true;
        } else {
            // Jump to click position
            setValueFromPos(event.localPos.x);
            m_dragging = true;
        }
        return true;
    }
    return false;
}

bool Slider::onMouseMove(MouseEvent& event) {
    if (m_dragging) {
        setValueFromPos(event.localPos.x);
    }
    return m_dragging;
}

bool Slider::onMouseUp(MouseEvent& event) {
    if (event.button == 0 && m_dragging) {
        m_dragging = false;
        return true;
    }
    return false;
}

bool Slider::onKeyDown(KeyEvent& event) {
    if (event.type == KeyEvent::Down) {
        float step = m_step > 0 ? m_step : (m_max - m_min) / 100.0f;
        if (event.key == 263) { // Left arrow (GLFW_KEY_RIGHT-1 in some layouts)
            // Actually GLFW_KEY_LEFT = 263, GLFW_KEY_RIGHT = 262
            setValue(m_value - step);
        } else if (event.key == 262) {
            setValue(m_value + step);
            if (m_onValueChanged) m_onValueChanged(m_value);
            return true;
        }
    }
    return false;
}

bool Slider::onMouseEnter(MouseEvent&) {
    m_hovered = true;
    return true;
}

bool Slider::onMouseLeave(MouseEvent&) {
    m_hovered = false;
    return true;
}

void Slider::onFocusGained() {
    m_focusRing = true;
}

void Slider::onFocusLost() {
    m_focusRing = false;
}

} // namespace ui

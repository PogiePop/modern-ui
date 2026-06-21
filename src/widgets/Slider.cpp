#include "Slider.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"

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
    float trackW = m_bounds.width - m_thumbSize;
    return m_thumbSize * 0.5f + normalizedValue() * trackW;
}

void Slider::setValueFromPos(float x) {
    float trackStart = m_thumbSize * 0.5f;
    float trackW = m_bounds.width - m_thumbSize;
    float t = (x - trackStart) / trackW;
    t = std::clamp(t, 0.0f, 1.0f);
    float newVal = m_min + t * (m_max - m_min);
    setValue(newVal);
    if (m_onValueChanged) m_onValueChanged(m_value);
}

Size Slider::measure(const Size& available) const {
    (void)available;
    return {200, m_thumbSize + 8};
}

void Slider::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;
    float trackY = sy + (m_bounds.height - m_trackHeight) * 0.5f;
    float thumbX = sx + thumbCenterX() - m_thumbSize * 0.5f;
    float thumbY = sy + (m_bounds.height - m_thumbSize) * 0.5f;

    Color trackColor = Color::fromHex(0xFF444444);
    Color activeColor = Color::fromHex(0xFF3B82F6);
    Color thumbColor = m_dragging ? Color::fromHex(0xFF2563EB)
                     : (m_hovered ? Color::fromHex(0xFF3B82F6) : Color::fromHex(0xFF60A5FA));
    Color focusColor = Color::fromHex(0xFFFFFFFF);

    if (m_theme) {
        trackColor = m_theme->color(ColorRole::BgTertiary);
        activeColor = m_useColorRole ? m_theme->color(m_colorRole) : m_theme->color(ColorRole::Primary);
        if (!m_dragging && !m_hovered) thumbColor = activeColor;
        else thumbColor = m_dragging ? m_theme->color(ColorRole::PrimaryActive) : m_theme->color(ColorRole::PrimaryHover);
        focusColor = m_theme->color(ColorRole::BorderFocused);
    }

    painter.drawRoundedRect({sx, trackY, m_bounds.width, m_trackHeight}, trackColor, m_trackHeight*0.5f);
    painter.drawRoundedRect({sx, trackY, thumbCenterX(), m_trackHeight}, activeColor, m_trackHeight*0.5f);
    Rect thumbRect{thumbX, thumbY, m_thumbSize, m_thumbSize};
    painter.drawRoundedRect(thumbRect, thumbColor, m_thumbSize*0.5f);
    if (m_focusRing) painter.drawRectOutline(thumbRect.insetBy(-2, -2), focusColor, 2);
}

bool Slider::onMouseDown(MouseEvent& event) {
    if (event.button == 0) {
        // Check if click is on thumb
        float thumbX = thumbCenterX() - m_thumbSize * 0.5f;
        float thumbY = (m_bounds.height - m_thumbSize) * 0.5f;
        Rect thumbRect{thumbX, thumbY, m_thumbSize, m_thumbSize};

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

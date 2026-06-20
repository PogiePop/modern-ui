#include "Widget.hpp"

// Forward-declare theme for later phases
namespace ui {

Widget::Widget() = default;

Widget::~Widget() = default;

void Widget::addChild(std::unique_ptr<Widget> child) {
    if (!child) return;
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

std::unique_ptr<Widget> Widget::removeChild(Widget* child) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if (it->get() == child) {
            auto result = std::move(*it);
            m_children.erase(it);
            result->m_parent = nullptr;
            return result;
        }
    }
    return nullptr;
}

void Widget::removeAllChildren() {
    for (auto& c : m_children) {
        c->m_parent = nullptr;
    }
    m_children.clear();
}

void Widget::setPosition(float x, float y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void Widget::setSize(float w, float h) {
    m_bounds.width = w;
    m_bounds.height = h;
}

void Widget::setBounds(const Rect& r) {
    m_bounds = r;
}

Point Widget::globalPosition() const {
    Point p = m_bounds.topLeft();
    for (const Widget* w = m_parent; w; w = w->m_parent) {
        p.x += w->m_bounds.x;
        p.y += w->m_bounds.y;
        p.x -= w->scrollOffsetX();
        p.y -= w->scrollOffsetY();
    }
    return p;
}

bool Widget::focused() const {
    // Will be set by App in later phases
    return false;
}

Theme& Widget::theme() const {
    // Walk up to find theme; return a static dummy for now
    for (const Widget* w = this; w; w = w->m_parent) {
        if (w->m_theme) return *w->m_theme;
    }
    static Theme* fallback = nullptr;
    // Will be resolved at runtime by Container/App
    return *fallback;
}

void Widget::layout() {
    // Default: size already set by parent, just layout children
    for (auto& child : m_children) {
        child->layout();
    }
}

Widget* Widget::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    // Check local bounds: localPoint is relative to this widget's top-left (0,0)
    if (localPoint.x < 0 || localPoint.y < 0 ||
        localPoint.x >= m_bounds.width || localPoint.y >= m_bounds.height) {
        return nullptr;
    }

    // Check children in reverse order (last child = top-most)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        Point childLocal = localPoint;
        childLocal.x -= (*it)->m_bounds.x;
        childLocal.y -= (*it)->m_bounds.y;
        Widget* hit = (*it)->hitTest(childLocal);
        if (hit) return hit;
    }
    return this; // leaf or container that catches all
}

// Default event handlers (do nothing, return false = not handled)
bool Widget::onMouseDown(MouseEvent&)  { return false; }
bool Widget::onMouseUp(MouseEvent&)    { return false; }
bool Widget::onMouseMove(MouseEvent&)  { return false; }
bool Widget::onMouseEnter(MouseEvent&) { return false; }
bool Widget::onMouseLeave(MouseEvent&) { return false; }
bool Widget::onKeyDown(KeyEvent&)      { return false; }
bool Widget::onKeyUp(KeyEvent&)        { return false; }
bool Widget::onCharInput(CharEvent&)   { return false; }
bool Widget::onScrollWheel(float, float) { return false; }
void Widget::onFocusGained() {}
void Widget::onFocusLost() {}

// ---- Animation ----

void Widget::fadeIn(float duration) {
    m_opacity = 0; m_opacityTarget = 1.0f;
    m_opacitySpeed = duration > 0 ? 1.0f / duration : 1000;
}

void Widget::fadeOut(float duration) {
    m_opacityTarget = 0;
    m_opacitySpeed = duration > 0 ? 1.0f / duration : 1000;
}

void Widget::slideIn(Point from, float duration) {
    m_slideFromX = from.x; m_slideFromY = from.y;
    m_slideElapsed = 0; m_slideDuration = duration;
    m_sliding = true;
}

bool Widget::isAnimating() const {
    return m_opacity != m_opacityTarget || m_sliding;
}

float Widget::slideOffsetX() const {
    if (!m_sliding) return 0;
    float t = m_slideDuration > 0 ? m_slideElapsed / m_slideDuration : 1;
    if (t > 1) t = 1;
    float ease = t * (2 - t); // easeOut
    return m_slideFromX * (1 - ease);
}

float Widget::slideOffsetY() const {
    if (!m_sliding) return 0;
    float t = m_slideDuration > 0 ? m_slideElapsed / m_slideDuration : 1;
    if (t > 1) t = 1;
    float ease = t * (2 - t);
    return m_slideFromY * (1 - ease);
}

void Widget::tickAnimation(float dt) {
    // Opacity animation
    if (m_opacity < m_opacityTarget)
        m_opacity = std::min(m_opacity + m_opacitySpeed * dt, m_opacityTarget);
    else if (m_opacity > m_opacityTarget)
        m_opacity = std::max(m_opacity - m_opacitySpeed * dt, m_opacityTarget);

    // Slide animation
    if (m_sliding) {
        m_slideElapsed += dt;
        if (m_slideElapsed >= m_slideDuration) m_sliding = false;
    }

    for (auto& c : m_children) c->tickAnimation(dt);
}

} // namespace ui

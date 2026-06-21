#include "Dialog.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"

namespace ui {

Dialog::Dialog() : Container(LayoutDirection::Vertical) {
    m_visible = false;
    setPadding({16, 16, 16, 16});
    setSpacing(8);
}

void Dialog::setTitle(const std::string& title) { m_title = title; }

void Dialog::show(Widget* parent) {
    m_parentWidget = parent;
    m_open = true;
    m_visible = true;

    // Default size centered on parent
    m_bounds = {100, 100, 400, 300};
    if (parent) {
        float pw = parent->bounds().width;
        float ph = parent->bounds().height;
        m_bounds = {(pw - 400) * 0.5f, (ph - 300) * 0.5f, 400, 300};
    }
}

void Dialog::close() {
    m_open = false;
    m_visible = false;
    if (m_onClose) m_onClose();
}

void Dialog::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;

    // Full-screen backdrop
    if (m_parentWidget) {
        Rect backdrop{0, 0, m_parentWidget->bounds().width, m_parentWidget->bounds().height};
        painter.drawRect(backdrop, Color{0, 0, 0, 0.5f});
    }

    // Dialog background
    Rect local{sx, sy, m_bounds.width, m_bounds.height};
    painter.drawRect(local, Color::fromHex(0xFF2A2A3E));
    painter.drawRectOutline(local, Color::fromHex(0xFF444466), 1);

    // Title bar
    Rect titleBar{sx, sy, m_bounds.width, TITLE_BAR_H};
    painter.drawRect(titleBar, Color::fromHex(0xFF1E1E32));
    painter.drawText(titleBar.insetBy(8, 0), m_title, Color::fromHex(0xFFFFFFFF), TextAlign::Left);

    // Content area
    for (auto& child : m_children) {
        if (!child->visible()) continue;
        child->paint(painter);
    }
}

Widget* Dialog::hitTest(Point localPoint) {
    if (!m_visible || !m_open) return nullptr;
    // Block all clicks (backdrop) — always return this
    return this;
}

bool Dialog::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    // Title bar drag
    if (e.localPos.y < TITLE_BAR_H) {
        m_dragging = true;
        m_dragOffsetX = e.localPos.x;
        m_dragOffsetY = e.localPos.y;
        return true;
    }
    // Click outside dialog bounds = close (local bounds: 0,0,w,h)
    if (e.localPos.x < 0 || e.localPos.y < 0 ||
        e.localPos.x >= m_bounds.width || e.localPos.y >= m_bounds.height) {
        close();
        return true;
    }
    return false;
}

bool Dialog::onKeyDown(KeyEvent& e) {
    if (e.type == KeyEvent::Down && e.key == 256) { // Escape
        close();
        return true;
    }
    return Container::onKeyDown(e);
}

} // namespace ui

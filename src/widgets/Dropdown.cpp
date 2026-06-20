#include "Dropdown.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"

namespace ui {

Dropdown::Dropdown() { m_focusable = true; m_tabIndex = 0; }

void Dropdown::addItem(const std::string& item) { m_items.push_back(item); }
void Dropdown::removeItem(size_t index) {
    if (index < m_items.size()) m_items.erase(m_items.begin() + index);
    if (m_selectedIndex >= m_items.size() && !m_items.empty()) m_selectedIndex = m_items.size() - 1;
}
void Dropdown::clearItems() { m_items.clear(); m_selectedIndex = 0; }

void Dropdown::setSelectedIndex(size_t index) {
    if (index < m_items.size()) {
        m_selectedIndex = index;
        if (m_onSelectedChanged) m_onSelectedChanged(index, m_items[index]);
    }
}

const std::string& Dropdown::selectedText() const {
    static const std::string empty;
    return m_selectedIndex < m_items.size() ? m_items[m_selectedIndex] : empty;
}

void Dropdown::setOnSelectedChanged(std::function<void(size_t, const std::string&)> cb) {
    m_onSelectedChanged = std::move(cb);
}

Rect Dropdown::popupRect() const {
    return {0, HEADER_HEIGHT, m_bounds.width, std::min(m_items.size() * ITEM_HEIGHT, 200.0f)};
}
Rect Dropdown::itemRect(size_t i) const {
    return {0, HEADER_HEIGHT + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
}

float Dropdown::popupOffsetY() const {
    if (m_expandDir == Down) return 0;
    float ph = popupRect().height;
    if (m_expandDir == Up) return -ph;
    // Auto: expand up if not enough space below
    Point g = globalPosition();
    float spaceBelow = 800 - (g.y + HEADER_HEIGHT); // approximate
    return (spaceBelow < ph) ? -ph : 0;
}

Rect Dropdown::screenRect() const {
    Point g = globalPosition();
    float po = popupOffsetY();
    float h = m_open ? HEADER_HEIGHT + popupRect().height : m_bounds.height;
    if (po < 0) return {g.x, g.y + po, m_bounds.width, h};
    return {g.x, g.y, m_bounds.width, h};
}

Size Dropdown::measure(const Size&) const { return {150, HEADER_HEIGHT}; }

void Dropdown::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;

    // Header
    painter.drawRect({sx, sy, m_bounds.width, HEADER_HEIGHT}, Color::fromHex(0xFF333344));
    painter.drawRectOutline({sx, sy, m_bounds.width, HEADER_HEIGHT}, Color::fromHex(0xFF555555), 1);

    // Arrow
    float ax = sx + m_bounds.width - 20, ay = sy + HEADER_HEIGHT * 0.5f;
    Color ac{0.7f,0.7f,0.7f,1};
    painter.drawRect({ax-4, ay-2, 8, 1}, ac);
    painter.drawRect({ax-3, ay-1, 6, 1}, ac);
    painter.drawRect({ax-2, ay, 4, 1}, ac);

    // Selected text
    if (!m_items.empty())
        painter.drawText({sx+6, sy, m_bounds.width-30, HEADER_HEIGHT}, m_items[m_selectedIndex],
                         Color::fromHex(0xFFCCCCCC), TextAlign::Left);

    // Popup
    if (m_open) {
        float ph = popupRect().height;
        float po = popupOffsetY();
        float py = sy + HEADER_HEIGHT + po;
        painter.drawRect({sx, py, m_bounds.width, ph}, Color::fromHex(0xFF2A2A3A));
        painter.drawRectOutline({sx, py, m_bounds.width, ph}, Color::fromHex(0xFF555566), 1);

        for (size_t i = 0; i < m_items.size(); ++i) {
            Rect r{sx, py + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
            // Highlight: selected (blue), hovered (gray), normal (transparent)
            if (i == m_selectedIndex)
                painter.drawRect(r, Color::fromHex(0xFF3B82F6));
            else if (i == m_hoveredIndex)
                painter.drawRect(r, Color::fromHex(0xFF444466));
            bool hi = (i == m_selectedIndex || i == m_hoveredIndex);
            painter.drawText(r.insetBy(6, 0), m_items[i],
                hi ? Color::fromHex(0xFFFFFFFF) : Color::fromHex(0xFFAAAAAA), TextAlign::Left);
        }
    }
}

bool Dropdown::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (e.localPos.y < HEADER_HEIGHT) {
        m_open = !m_open;
        setZIndex(m_open ? 999 : 0); // subtree max-z pushes popup above everything
        return true;
    }
    if (m_open) {
        for (size_t i = 0; i < m_items.size(); ++i)
            if (itemRect(i).contains(e.localPos)) { setSelectedIndex(i); m_open = false; setZIndex(0); return true; }
        m_open = false; setZIndex(0); return true;
    }
    return false;
}

bool Dropdown::onMouseMove(MouseEvent& e) {
    if (!m_open) return false;
    m_hoveredIndex = m_items.size();
    for (size_t i = 0; i < m_items.size(); ++i)
        if (itemRect(i).contains(e.localPos)) { m_hoveredIndex = i; break; }
    return true;
}

Widget* Dropdown::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x < 0) return nullptr;
    float po = popupOffsetY();
    float minY = po < 0 ? po : 0;
    float maxY = m_open ? HEADER_HEIGHT + popupRect().height + (po < 0 ? po : 0) : m_bounds.height;
    if (localPoint.y < minY || localPoint.y > maxY) return nullptr;
    if (localPoint.x < m_bounds.width) return this;
    return nullptr;
}

} // namespace ui

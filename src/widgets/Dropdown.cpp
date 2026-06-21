#include "Dropdown.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"

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
    static const std::string kEmpty;
    return hasSelection() ? m_items[m_selectedIndex] : kEmpty;
}

void Dropdown::clearSelection() {
    m_selectedIndex = m_items.size(); // sentinel = no selection
}
void Dropdown::setOnSelectedChanged(std::function<void(size_t, const std::string&)> cb) {
    m_onSelectedChanged = std::move(cb);
}

// ── Layout helpers ──
Rect Dropdown::popupRect() const {
    return {0, HEADER_HEIGHT, m_bounds.width, std::min(m_items.size() * ITEM_HEIGHT, 200.0f)};
}

float Dropdown::popupOffsetY() const {
    if (m_expandDir == Down) return 0;
    float ph = popupRect().height;
    if (m_expandDir == Up) return -ph;
    Point g = globalPosition();
    float viewH = 600.0f;
    if (m_parent) viewH = m_parent->bounds().height;
    float spaceBelow = viewH - (g.y + HEADER_HEIGHT);
    return (spaceBelow < ph) ? -ph : 0;
}

// Where does the popup start in screenRect-local coordinates?
float Dropdown::popupLocalY() const {
    float po = popupOffsetY();
    return (po < 0) ? 0.0f : HEADER_HEIGHT;
}

// Where does the header start in screenRect-local coordinates?
float Dropdown::headerLocalY() const {
    float po = popupOffsetY();
    return (po < 0) ? -po : 0.0f;
}

Rect Dropdown::itemRect(size_t i) const {
    float py = popupLocalY();
    return {0, py + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
}

Rect Dropdown::screenRect() const {
    Point g = globalPosition();
    float po = popupOffsetY();
    float h = m_open ? HEADER_HEIGHT + popupRect().height : m_bounds.height;
    if (po < 0) return {g.x, g.y + po, m_bounds.width, h};
    return {g.x, g.y, m_bounds.width, h};
}

Size Dropdown::measure(const Size&) const { return {150, HEADER_HEIGHT}; }

// ── Paint ──
void Dropdown::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;

    // Header — draw at headerLocalY offset from screenRect top
    float hy = sy + headerLocalY();

    Color headerBg = Color::fromHex(0xFF333344);
    Color headerBorder = Color::fromHex(0xFF555555);
    Color textCol = Color::fromHex(0xFFCCCCCC);
    Color popupBg = Color::fromHex(0xFF2A2A3A);
    Color popupBorder = Color::fromHex(0xFF555566);
    Color selectedBg = Color::fromHex(0xFF3B82F6);
    Color hoveredBg = Color::fromHex(0xFF444466);

    if (m_theme) {
        headerBg = m_theme->color(ColorRole::Surface);
        headerBorder = m_theme->color(ColorRole::Border);
        textCol = m_theme->color(ColorRole::Text);
        popupBg = m_theme->color(ColorRole::BgSecondary);
        popupBorder = m_theme->color(ColorRole::Border);
        selectedBg = m_theme->color(ColorRole::Primary);
        hoveredBg = m_theme->color(ColorRole::SurfaceHover);
    }

    painter.drawRect({sx, hy, m_bounds.width, HEADER_HEIGHT}, headerBg);
    painter.drawRectOutline({sx, hy, m_bounds.width, HEADER_HEIGHT}, headerBorder, 1);

    // Arrow indicator
    float ax = sx + m_bounds.width - 16, ay = hy + HEADER_HEIGHT * 0.5f;
    Color ac{0.7f, 0.7f, 0.7f, 1};
    painter.drawRect({ax - 4, ay - 2, 8, 1}, ac);
    painter.drawRect({ax - 3, ay - 1, 6, 1}, ac);
    painter.drawRect({ax - 2, ay, 4, 1}, ac);

    // Clear X button (when has selection and not open)
    bool sel = hasSelection();
    float textW = m_bounds.width - 40;
    if (sel) {
        textW = m_bounds.width - 52; // make room for X
        float cx = ax - 18, cy = hy + HEADER_HEIGHT * 0.5f;
        if (!m_open) {
            Color xCol{0.5f, 0.5f, 0.55f, 1};
            if (m_theme) xCol = m_theme->color(ColorRole::TextDisabled);
            painter.drawRect({cx - 3, cy - 1, 6, 2}, xCol);
            painter.drawRect({cx - 1, cy - 3, 2, 6}, xCol);
        }
    }

    // Selected text or placeholder in header
    if (!m_items.empty()) {
        Color tc = sel ? textCol : Color{0.5f, 0.5f, 0.55f, 1};
        if (m_theme && !sel) tc = m_theme->color(ColorRole::TextDisabled);
        const std::string& display = sel ? m_items[m_selectedIndex] : m_items[0];
        std::string label = sel ? display : (display + " ...");
        painter.drawText({sx + 6, hy, textW, HEADER_HEIGHT}, label, tc, TextAlign::Left);
    }

    // Popup items
    if (m_open) {
        float ph = popupRect().height;
        float py = sy + popupLocalY();
        painter.drawRect({sx, py, m_bounds.width, ph}, popupBg);
        painter.drawRectOutline({sx, py, m_bounds.width, ph}, popupBorder, 1);

        for (size_t i = 0; i < m_items.size(); ++i) {
            Rect r{sx, py + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
            if (i == m_selectedIndex)
                painter.drawRect(r, selectedBg);
            else if (i == m_hoveredIndex)
                painter.drawRect(r, hoveredBg);
            bool hi = (i == m_selectedIndex || i == m_hoveredIndex);
            painter.drawText(r.insetBy(6, 0), m_items[i],
                hi ? Color::fromHex(0xFFFFFFFF) : Color::fromHex(0xFFAAAAAA), TextAlign::Left);
        }
    }
}

// ── Mouse: use screenRect-local coordinates consistently ──
bool Dropdown::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;

    float hy = headerLocalY();
    if (e.localPos.y >= hy && e.localPos.y < hy + HEADER_HEIGHT) {
        // Check for clear X click (right side of header, before arrow)
        float clearX = m_bounds.width - 38; // X is between text and arrow
        if (!m_open && hasSelection() && e.localPos.x >= clearX && e.localPos.x < m_bounds.width - 16) {
            clearSelection();
            if (m_onSelectedChanged) m_onSelectedChanged(m_items.size(), "");
            return true;
        }
        m_open = !m_open;
        setZIndex(m_open ? 999 : 0);
        return true;
    }
    if (m_open) {
        float py = popupLocalY();
        for (size_t i = 0; i < m_items.size(); ++i) {
            Rect r{0, py + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
            if (r.contains(e.localPos)) {
                setSelectedIndex(i);
                m_open = false;
                setZIndex(0);
                return true;
            }
        }
        m_open = false; setZIndex(0); return true;
    }
    return false;
}

bool Dropdown::onMouseMove(MouseEvent& e) {
    if (!m_open) return false;
    m_hoveredIndex = m_items.size();
    float py = popupLocalY();
    for (size_t i = 0; i < m_items.size(); ++i) {
        Rect r{0, py + i * ITEM_HEIGHT, m_bounds.width, ITEM_HEIGHT};
        if (r.contains(e.localPos)) { m_hoveredIndex = i; break; }
    }
    return true;
}

// ── hitTest: receives localPoint relative to m_bounds.topLeft() ──
Widget* Dropdown::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x < 0 || localPoint.x >= m_bounds.width) return nullptr;
    float po = popupOffsetY();
    float minY = po < 0 ? po : 0;
    float maxY = m_open ? HEADER_HEIGHT + popupRect().height + (po < 0 ? po : 0) : m_bounds.height;
    if (localPoint.y < minY || localPoint.y > maxY) return nullptr;
    return this;
}

} // namespace ui

#include "ContextMenu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "core/App.hpp"
#include "res/Theme.hpp"
#include "res/Font.hpp"

namespace ui {

ContextMenu::ContextMenu() { m_visible = false; m_focusable = true; setZIndex(10000); }

void ContextMenu::setItems(const std::vector<MenuAction>& items) { m_items = items; }
void ContextMenu::addItem(const std::string& label, std::function<void()> action,
                          const std::string& shortcut, bool enabled) {
    m_items.push_back({label, shortcut, std::move(action), enabled, false});
}
void ContextMenu::addSeparator() { m_items.push_back({"", "", nullptr, false, true}); }
void ContextMenu::clearItems() { m_items.clear(); }

bool ContextMenu::show(float sx, float sy, Widget* parent) {
    if (m_items.empty()) return false;
    m_x = sx; m_y = sy;
    m_parentWidget = parent;
    m_open = true; m_visible = true;
    m_hovered = -1;
    float h = totalHeight();
    m_bounds = {m_x, m_y, 180, h + 4};
    if (App::instance()) App::instance()->setOverlay(this);
    return true;
}

void ContextMenu::dismiss() {
    m_open = false; m_visible = false; m_hovered = -1;
    if (App::instance() && App::instance()->overlay() == this)
        App::instance()->setOverlay(nullptr);
}

Rect ContextMenu::screenRect() const { return m_bounds; }

Size ContextMenu::measure(const Size&) const { return {180, totalHeight() + 4}; }

float ContextMenu::totalHeight() const {
    float h = 0;
    for (auto& item : m_items) h += item.separator ? SEPARATOR_H : ITEM_H;
    return h;
}

int ContextMenu::itemAt(float localY) const {
    float y = 2;
    for (int i = 0; i < (int)m_items.size(); ++i) {
        float h = m_items[i].separator ? SEPARATOR_H : ITEM_H;
        if (localY >= y && localY < y + h) return m_items[i].separator ? -1 : i;
        y += h;
    }
    return -1;
}

void ContextMenu::paint(Painter& p) {
    if (!m_open) return;
    Rect r = screenRect();

    // Background
    Color bg = Color::fromHex(0xDD1E1E2E);
    Color borderCol = Color::fromHex(0xFF3E3E4E);
    Color textCol = Color::fromHex(0xFFCCCCCC);
    Color hoverBg = Color::fromHex(0xFF3B82F6);
    Color shortcutCol = Color::fromHex(0xFF666677);
    Color separatorCol = Color::fromHex(0xFF2E2E3E);
    Color disabledCol = Color::fromHex(0xFF555566);

    if (m_theme) {
        bg = m_theme->color(ColorRole::Surface);
        bg.a = 0.95f;
        borderCol = m_theme->color(ColorRole::Border);
        textCol = m_theme->color(ColorRole::Text);
        hoverBg = m_theme->color(ColorRole::Primary);
        shortcutCol = m_theme->color(ColorRole::TextDisabled);
        separatorCol = m_theme->color(ColorRole::Border);
        disabledCol = m_theme->color(ColorRole::TextDisabled);
    }

    p.drawRoundedRect(r, bg, 6);
    p.drawRectOutline(r, borderCol, 1);
    p.drawShadow(r, 4, Color{0, 0, 0, 0.3f}, {0, 4}, 12);

    float lh = m_font ? m_font->lineHeight() : 16;
    float y = r.y + 2;
    for (int i = 0; i < (int)m_items.size(); ++i) {
        auto& item = m_items[i];
        if (item.separator) {
            p.drawRect({r.x + 8, y + SEPARATOR_H * 0.5f, r.width - 16, 1}, separatorCol);
            y += SEPARATOR_H;
        } else {
            if (i == m_hovered && item.enabled) {
                p.drawRoundedRect({r.x + 4, y, r.width - 8, ITEM_H}, hoverBg, 4);
            }
            Color tc = item.enabled ? textCol : disabledCol;
            p.drawText({r.x + 12, y + (ITEM_H - lh) * 0.5f, r.width - 80, lh}, item.label, tc, TextAlign::Left);
            if (!item.shortcut.empty()) {
                p.drawText({r.x + r.width - 70, y + (ITEM_H - lh) * 0.5f, 58, lh},
                           item.shortcut, shortcutCol, TextAlign::Right);
            }
            y += ITEM_H;
        }
    }
}

bool ContextMenu::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    int idx = itemAt(e.localPos.y);
    if (idx >= 0 && idx < (int)m_items.size()) {
        auto& item = m_items[idx];
        if (!item.separator && item.enabled && item.action) {
            item.action();
            dismiss();
            return true;
        }
    }
    // Click outside items — still dismiss
    dismiss();
    return true;
}

bool ContextMenu::onMouseMove(MouseEvent& e) {
    m_hovered = itemAt(e.localPos.y);
    return true;
}

bool ContextMenu::onKeyDown(KeyEvent& e) {
    if (e.isEscape()) { dismiss(); return true; }
    return false;
}

Widget* ContextMenu::hitTest(Point localPoint) {
    if (!m_open) return nullptr;
    if (localPoint.x >= 0 && localPoint.x < m_bounds.width &&
        localPoint.y >= 0 && localPoint.y < m_bounds.height)
        return this;
    return nullptr;
}

} // namespace ui

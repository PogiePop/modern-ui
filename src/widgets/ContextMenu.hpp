#pragma once
#include <functional>
#include <string>
#include <vector>
#include "Widget.hpp"

namespace ui {

struct MenuAction {
    std::string label;
    std::string shortcut; // e.g. "Ctrl+C" — display only
    std::function<void()> action;
    bool enabled = true;
    bool separator = false; // if true, renders as separator line
};

// Right-click context menu — rendered as an overlay at click position
class ContextMenu : public Widget {
public:
    ContextMenu();

    void setItems(const std::vector<MenuAction>& items);
    void addItem(const std::string& label, std::function<void()> action,
                 const std::string& shortcut = "", bool enabled = true);
    void addSeparator();
    void clearItems();

    // Show at screen position, returns true if any item is available
    bool show(float screenX, float screenY, Widget* parent);
    void dismiss();

    bool isOpen() const { return m_open; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "ContextMenu"; }

    // Overrides
    Rect screenRect() const;
    Size measure(const Size&) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;
    bool onKeyDown(KeyEvent& e) override;
    Widget* hitTest(Point localPoint) override;

private:
    std::vector<MenuAction> m_items;
    int m_hovered = -1;
    bool m_open = false;
    float m_x = 0, m_y = 0;
    class Font* m_font = nullptr;
    Widget* m_parentWidget = nullptr;

    static constexpr float ITEM_H = 26;
    static constexpr float SEPARATOR_H = 5;
    float totalHeight() const;
    int itemAt(float localY) const;
};

} // namespace ui

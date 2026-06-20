#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "Widget.hpp"
#include "core/Icon.hpp"

namespace ui {

// A single menu item (can have children for sub-menus)
struct MenuItemData {
    std::string label;
    IconType icon = IconType::None;
    std::function<void()> onClick;
    bool enabled = true;
    std::vector<MenuItemData> children; // sub-items for expandable menu
};

// Sidebar/header navigation menu with multi-level expand/collapse
class NavMenu : public Widget {
public:
    NavMenu();

    void addItem(const std::string& label, IconType icon, std::function<void()> onClick);
    void addSubMenu(const std::string& label, IconType icon,
                    const std::vector<MenuItemData>& children);
    void clearItems();
    void setFont(class Font* f) { m_font = f; }
    void setOnSelect(std::function<void(const std::string&)> cb) { m_onSelect = std::move(cb); }

    const char* typeName() const override { return "NavMenu"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;
    Widget* hitTest(Point lp) override;

private:
    struct Node {
        MenuItemData data;
        std::vector<std::unique_ptr<Node>> children;
        bool expanded = false;
        float cachedHeight = 0;
    };
    std::vector<std::unique_ptr<Node>> m_roots;
    class Font* m_font = nullptr;
    std::function<void(const std::string&)> m_onSelect;
    int m_hoveredPath = -1;
    std::vector<int> m_hoveredSub; // path to hovered item
    float m_totalHeight = 0;
    static constexpr float ITEM_H = 32, INDENT = 20;

    float paintNode(Painter& p, float x, float y, float w, const Node& node, int depth, int& idx, int hoverFlat, const std::vector<int>& hoverPath, int pathDepth);
    Node* findNode(int flatIdx, int& current, std::vector<int>& path);
    void toggleExpand(int flatIdx);
    void computeHeight(Node& node);
};

} // namespace ui

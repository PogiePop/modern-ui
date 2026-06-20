#include "Menu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include <algorithm>
#include <stack>

namespace ui {

NavMenu::NavMenu() { m_focusable = true; }

void NavMenu::addItem(const std::string& label, IconType icon, std::function<void()> onClick) {
    auto n = std::make_unique<Node>();
    n->data = {label, icon, std::move(onClick), true, {}};
    m_roots.push_back(std::move(n));
    m_totalHeight += ITEM_H;
}

void NavMenu::addSubMenu(const std::string& label, IconType icon, const std::vector<MenuItemData>& children) {
    auto n = std::make_unique<Node>();
    n->data = {label, icon, nullptr, true, children};
    for (auto& ch : children) {
        auto cn = std::make_unique<Node>();
        cn->data = ch;
        n->children.push_back(std::move(cn));
    }
    computeHeight(*n);
    m_totalHeight += ITEM_H; // header + children when expanded
    m_roots.push_back(std::move(n));
}

void NavMenu::clearItems() { m_roots.clear(); m_totalHeight = 0; }

void NavMenu::computeHeight(Node& node) {
    node.cachedHeight = ITEM_H;
    if (node.expanded) for (auto& c : node.children) node.cachedHeight += ITEM_H;
}

Size NavMenu::measure(const Size& a) const {
    float h = m_totalHeight;
    for (auto& r : m_roots) if (r->expanded) for (auto& c : r->children) h += ITEM_H;
    return {a.width > 0 ? a.width : 200, std::max(h, 100.0f)};
}

NavMenu::Node* NavMenu::findNode(int flatIdx, int& current, std::vector<int>& path) {
    std::function<Node*(const std::vector<std::unique_ptr<Node>>&)> search =
    [&](const std::vector<std::unique_ptr<Node>>& nodes) -> Node* {
        for (size_t i = 0; i < nodes.size(); ++i) {
            path.push_back((int)i);
            if (current == flatIdx) return nodes[i].get();
            current++;
            auto* found = search(nodes[i]->children);
            if (found) return found;
            path.pop_back();
        }
        return nullptr;
    };
    int cur = 0;
    return search(m_roots);
}

void NavMenu::toggleExpand(int flatIdx) {
    std::vector<int> path; int cur = 0;
    Node* n = findNode(flatIdx, cur, path);
    if (n && !n->children.empty()) {
        n->expanded = !n->expanded;
        computeHeight(*n);
    }
}

float NavMenu::paintNode(Painter& p, float x, float y, float w, const Node& node, int depth, int& idx, int hoverFlat, const std::vector<int>& hoverPath, int pathDepth) {
    bool isHovered = (idx == hoverFlat);
    float lh = m_font ? m_font->lineHeight() : 18;
    float cx = x + depth * INDENT;

    // Background highlight
    if (isHovered) p.drawRect({cx, y, w - depth*INDENT, ITEM_H}, Color{0.2f,0.3f,0.5f,0.5f});
    else if (node.data.children.empty() && node.data.onClick == nullptr) {}

    // Icon
    if (node.data.icon != IconType::None)
        drawIcon(p, node.data.icon, cx+4, y+8, 16, isHovered ? Color{1,1,1,1} : Color{0.6f,0.6f,0.7f,1});

    // Expand arrow for sub-menus
    float textX = cx + (node.data.icon != IconType::None ? 26 : 8);
    if (!node.data.children.empty()) {
        std::string arrow = node.expanded ? "v" : ">";
        p.drawText({cx + w - depth*INDENT - 20, y+2, 16, ITEM_H-4}, arrow, Color{0.5f,0.5f,0.5f,1}, TextAlign::Center);
    }

    // Label
    Color tc = node.data.enabled ? (isHovered ? Color{1,1,1,1} : Color{0.8f,0.8f,0.8f,1}) : Color{0.4f,0.4f,0.4f,1};
    p.drawText({textX, y+2, w - depth*INDENT - textX - 20, ITEM_H-4}, node.data.label, tc, TextAlign::Left);

    float ny = y + ITEM_H;
    idx++;

    // Children (if expanded)
    if (node.expanded) {
        for (size_t ci = 0; ci < node.children.size(); ++ci) {
            std::vector<int> cp = hoverPath;
            bool childHovered = (pathDepth < (int)hoverPath.size() && ci == (size_t)hoverPath[pathDepth] && (int)ci < (int)node.children.size());
            ny = paintNode(p, x, ny, w, *node.children[ci], depth+1, idx, hoverFlat, hoverPath, pathDepth+1);
        }
    }
    return ny;
}

void NavMenu::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    float y = r.y + 2;
    int idx = 0;
    for (auto& root : m_roots) {
        std::vector<int> hp;
        y = paintNode(p, r.x, y, m_bounds.width, *root, 0, idx, m_hoveredPath, hp, 0);
    }
}

bool NavMenu::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    int clicked = (int)(e.localPos.y / ITEM_H);
    // Toggle expand or fire click
    std::vector<int> path; int cur = 0;
    Node* n = findNode(clicked, cur, path);
    if (n) {
        if (!n->children.empty()) toggleExpand(clicked);
        else if (n->data.onClick) n->data.onClick();
        if (m_onSelect) m_onSelect(n->data.label);
    }
    return true;
}

bool NavMenu::onMouseMove(MouseEvent& e) {
    m_hoveredPath = (int)(e.localPos.y / ITEM_H);
    return true;
}

Widget* NavMenu::hitTest(Point lp) {
    if (!m_visible) return nullptr;
    if (lp.x >= 0 && lp.y >= 0 && lp.x < m_bounds.width && lp.y < m_bounds.height) return this;
    return nullptr;
}

} // namespace ui

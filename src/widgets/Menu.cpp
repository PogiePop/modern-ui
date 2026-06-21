#include "Menu.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"

namespace ui {

NavMenu::NavMenu() { m_focusable = true; }

void NavMenu::buildTree(const std::vector<MenuItem>& items, std::vector<std::unique_ptr<Node>>& out) {
    for (auto& it : items) {
        auto n = std::make_unique<Node>(); n->item = it;
        if (!it.children.empty()) buildTree(it.children, n->kids);
        out.push_back(std::move(n));
    }
}

void NavMenu::setItems(const std::vector<MenuItem>& items) {
    m_roots.clear(); buildTree(items, m_roots);
    markLayoutDirty();
}

float NavMenu::totalHeight(const std::vector<std::unique_ptr<NavMenu::Node>>& nodes) const {
    float h = 0;
    for (auto& n : nodes) { h += ROW; if (n->open) h += totalHeight(n->kids); }
    return h;
}

Size NavMenu::measure(const Size& a) const {
    return {a.width > 0 ? a.width : 180, std::max(totalHeight(m_roots) + 4, 60.0f)};
}

NavMenu::Node* NavMenu::nodeAt(int& idx, std::vector<std::unique_ptr<Node>>& nodes) {
    for (auto& n : nodes) {
        if (idx == 0) return n.get(); idx--;
        if (n->open) { Node* f = nodeAt(idx, n->kids); if (f) return f; }
    }
    return nullptr;
}

float NavMenu::paintTree(Painter& p, const std::vector<std::unique_ptr<NavMenu::Node>>& nodes, float x, float y, int depth, int& idx) {
    float lh = m_font ? m_font->lineHeight() : 18;
    for (auto& n : nodes) {
        bool hov = (idx == m_hover);
        float cx = x + depth * INDENT;
        if (hov) p.drawRect({cx, y, m_bounds.width - depth*INDENT, ROW}, Color{0.2f,0.3f,0.5f,0.5f});
        // Icon
        float tx = cx + 4;
        if (n->item.icon != IconType::None) { drawIcon(p, n->item.icon, tx, y+7, 16, hov?Color{1,1,1,1}:Color{0.6f,0.6f,0.7f,1}); tx += 20; }
        // Arrow
        if (!n->kids.empty())
            p.drawText({cx + m_bounds.width - depth*INDENT - 20, y, 16, ROW}, n->open ? "v" : ">", Color{0.5f,0.5f,0.5f,1}, TextAlign::Center);
        // Label
        Color tc = n->item.enabled ? (hov ? Color{1,1,1,1} : Color{0.8f,0.8f,0.8f,1}) : Color{0.4f,0.4f,0.4f,1};
        p.drawText({tx, y+2, m_bounds.width - depth*INDENT - tx - 20, ROW-4}, n->item.label, tc, TextAlign::Left);
        y += ROW; idx++;
        if (n->open) y = paintTree(p, n->kids, x, y, depth+1, idx);
    }
    return y;
}

void NavMenu::paint(Painter& p) {
    int idx = 0;
    paintTree(p, m_roots, screenRect().x, screenRect().y + 2, 0, idx);
}

bool NavMenu::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    int idx = (int)(e.localPos.y / ROW);
    Node* n = nodeAt(idx, m_roots);
    if (n) {
        if (!n->kids.empty()) n->open = !n->open; // toggle
        else if (n->item.action) n->item.action();
    }
    return true;
}

bool NavMenu::onMouseMove(MouseEvent& e) { m_hover = (int)(e.localPos.y / ROW); return true; }

} // namespace ui

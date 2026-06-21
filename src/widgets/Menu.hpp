#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "Widget.hpp"
#include "core/Icon.hpp"

namespace ui {

struct MenuItem {
    std::string label; IconType icon = IconType::None;
    std::function<void()> action; bool enabled = true;
    std::vector<MenuItem> children; // if non-empty, this is expandable
};

class NavMenu : public Widget {
public:
    NavMenu();
    void setItems(const std::vector<MenuItem>& items); // replace all
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "NavMenu"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;

private:
    struct Node { MenuItem item; std::vector<std::unique_ptr<Node>> kids; bool open = false; };
    std::vector<std::unique_ptr<Node>> m_roots;
    class Font* m_font = nullptr;
    int m_hover = -1; // flat index

    void buildTree(const std::vector<MenuItem>& items, std::vector<std::unique_ptr<Node>>& out);
    float paintTree(Painter& p, const std::vector<std::unique_ptr<Node>>& nodes, float x, float y, int depth, int& idx);
    Node* nodeAt(int& idx, std::vector<std::unique_ptr<Node>>& nodes);
    float totalHeight(const std::vector<std::unique_ptr<Node>>& nodes) const;
    static constexpr float ROW = 30, INDENT = 18;
};

} // namespace ui

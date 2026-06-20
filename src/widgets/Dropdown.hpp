#pragma once

#include <functional>
#include <string>
#include <vector>

#include "Widget.hpp"

namespace ui {

class Dropdown : public Widget {
public:
    Dropdown();

    void addItem(const std::string& item);
    void removeItem(size_t index);
    void clearItems();

    size_t selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(size_t index);
    const std::string& selectedText() const;

    void setOnSelectedChanged(std::function<void(size_t, const std::string&)> cb);

    enum ExpandDir { Down, Up, Auto };
    void setExpandDir(ExpandDir d) { m_expandDir = d; }

    const char* typeName() const override { return "Dropdown"; }
    Rect screenRect() const;

    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& event) override;
    bool onMouseMove(MouseEvent& event) override;
    Widget* hitTest(Point localPoint) override;

private:
    std::vector<std::string> m_items;
    size_t m_selectedIndex = 0;
    size_t m_hoveredIndex = 0;
    bool m_open = false;
    ExpandDir m_expandDir = Down;
    std::function<void(size_t, const std::string&)> m_onSelectedChanged;

    static constexpr float ITEM_HEIGHT = 24;
    static constexpr float HEADER_HEIGHT = 28;

    Rect popupRect() const;
    Rect itemRect(size_t i) const;
    float popupOffsetY() const; // negative if expanding up
};

} // namespace ui

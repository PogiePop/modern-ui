#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include "Widget.hpp"

namespace ui {

class TabView : public Widget {
public:
    TabView();

    size_t addTab(const std::string& title, std::unique_ptr<Widget> content);
    void removeTab(size_t index);
    void setCurrentTab(size_t index);
    size_t currentTab() const { return m_currentTab; }
    size_t tabCount() const { return m_titles.size(); }

    void setOnTabChanged(std::function<void(size_t)> cb);

    const char* typeName() const override { return "TabView"; }

    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& event) override;
    Widget* hitTest(Point localPoint) override;

private:
    std::vector<std::string> m_titles;
    size_t m_currentTab = 0;
    std::function<void(size_t)> m_onTabChanged;

    static constexpr float TAB_HEIGHT = 32;
    static constexpr float TAB_PADDING = 12;
    static constexpr float TAB_GAP = 2;

    Rect tabRect(size_t i) const;
    void switchTo(size_t index);
};

} // namespace ui

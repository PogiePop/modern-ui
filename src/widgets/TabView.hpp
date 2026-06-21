#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "Widget.hpp"

namespace ui {

class Container;
class ScrollArea;

class TabView : public Widget {
public:
    TabView();

    size_t addTab(const std::string& title, std::unique_ptr<Widget> content);
    void removeTab(size_t index);
    void setCurrentTab(size_t index);
    size_t currentTab() const { return m_currentTab; }
    size_t tabCount() const { return m_tabContents.size(); }
    void setOnTabChanged(std::function<void(size_t)> cb);
    void setFont(class Font* f);

    // Content area border (drawn around the area below tabs)
    void setContentBorder(const Color& color, float thickness = 1.0f);
    void setContentBorderColor(const Color& color) { m_contentBorderColor = color; m_hasContentBorder = true; }
    void setContentBorderThickness(float t) { m_contentBorderThickness = t; }
    void clearContentBorder() { m_hasContentBorder = false; }
    bool hasContentBorder() const { return m_hasContentBorder; }
    Color contentBorderColor() const { return m_contentBorderColor; }

    // Tab bar background
    void setTabBarColor(const Color& c) { m_tabBarColor = c; m_hasTabBarColor = true; }
    void clearTabBarColor() { m_hasTabBarColor = false; }

    const char* typeName() const override { return "TabView"; }
    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onScrollWheel(float dx, float dy) override;
    Widget* hitTest(Point localPoint) override;

    // Access scroll area
    ScrollArea* scrollArea() const { return m_scrollArea; }

private:
    static constexpr float TAB_HEIGHT = 32;
    static constexpr float TAB_PADDING = 12;
    static constexpr float TAB_GAP = 2;

    Container* m_tabBar = nullptr;
    ScrollArea* m_scrollArea = nullptr;
    Container* m_contentHost = nullptr;

    struct TabState { float scrollX = 0; float scrollY = 0; };
    std::vector<TabState> m_tabStates;
    std::vector<Widget*> m_tabContents;
    std::vector<std::unique_ptr<Widget>> m_pendingContents;
    std::vector<std::string> m_titles;
    size_t m_currentTab = 0;
    std::function<void(size_t)> m_onTabChanged;
    class Font* m_font = nullptr;

    // Content border
    bool m_hasContentBorder = false;
    Color m_contentBorderColor{0.25f, 0.25f, 0.35f, 1.0f};
    float m_contentBorderThickness = 1.0f;

    // Tab bar customization
    bool m_hasTabBarColor = false;
    Color m_tabBarColor{0, 0, 0, 0};

    Rect tabRect(size_t i) const;
    void switchTo(size_t index);
};

} // namespace ui

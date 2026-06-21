#include "TabView.hpp"
#include "Container.hpp"
#include "ScrollArea.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
#include <algorithm>

namespace ui {

// Walk tree and set theme on all descendants
static void propagateTheme(Widget* w, Theme* theme) {
    if (!w || !theme) return;
    w->setTheme(theme);
    for (size_t i = 0; i < w->childCount(); ++i)
        propagateTheme(w->childAt(i), theme);
}

TabView::TabView() { m_focusable = false; }
void TabView::setFont(Font* f) { m_font = f; }
void TabView::setContentBorder(const Color& color, float thickness) {
    m_contentBorderColor = color;
    m_contentBorderThickness = thickness;
    m_hasContentBorder = true;
}

size_t TabView::addTab(const std::string& title, std::unique_ptr<Widget> content) {
    m_titles.push_back(title);
    m_tabStates.push_back({0, 0});
    Widget* raw = content.get();
    m_tabContents.push_back(raw);
    if (m_contentHost) {
        content->setVisible(m_tabContents.size() == 1);
        m_contentHost->addChild(std::move(content));
    } else {
        m_pendingContents.push_back(std::move(content));
    }
    markLayoutDirty();
    return m_titles.size() - 1;
}

void TabView::removeTab(size_t index) {
    if (index >= m_titles.size()) return;
    if (m_contentHost && index < m_tabContents.size() && m_tabContents[index])
        m_contentHost->removeChild(m_tabContents[index]);
    m_titles.erase(m_titles.begin() + index);
    m_tabContents.erase(m_tabContents.begin() + index);
    m_tabStates.erase(m_tabStates.begin() + index);
    if (m_currentTab >= m_titles.size()) m_currentTab = m_titles.size() > 0 ? m_titles.size()-1 : 0;
    for (size_t i = 0; i < m_tabContents.size(); ++i)
        if (m_tabContents[i]) m_tabContents[i]->setVisible(i == m_currentTab);
    markLayoutDirty();
}

void TabView::setCurrentTab(size_t index) { switchTo(index); }
void TabView::setOnTabChanged(std::function<void(size_t)> cb) { m_onTabChanged = std::move(cb); }

Rect TabView::tabRect(size_t i) const {
    float x = 0;
    for (size_t j = 0; j < i; ++j) x += m_titles[j].size()*8.0f + TAB_PADDING*2 + TAB_GAP;
    return {x, 0, m_titles[i].size()*8.0f + TAB_PADDING*2, TAB_HEIGHT};
}

void TabView::switchTo(size_t index) {
    if (index >= m_titles.size() || index == m_currentTab) return;
    if (m_currentTab < m_tabContents.size() && m_tabContents[m_currentTab])
        m_tabContents[m_currentTab]->setVisible(false);
    m_currentTab = index;
    if (m_currentTab < m_tabContents.size() && m_tabContents[m_currentTab])
        m_tabContents[m_currentTab]->setVisible(true);
    if (m_onTabChanged) m_onTabChanged(m_currentTab);
    markLayoutDirty();
}

Size TabView::measure(const Size& available) const { return {available.width, available.height}; }

void TabView::layout() {
    if (!m_tabBar) {
        auto bar = std::make_unique<Container>(LayoutDirection::Horizontal);
        m_tabBar = bar.get();
        m_tabBar->setMinSize(0, TAB_HEIGHT);
        addChild(std::move(bar));
    }
    if (!m_scrollArea) {
        auto scroll = std::make_unique<ScrollArea>();
        m_scrollArea = scroll.get();
        auto host = std::make_unique<Container>(LayoutDirection::Vertical);
        m_contentHost = host.get();
        m_contentHost->setPadding({4,4,4,4});
        m_scrollArea->content(std::move(host));
        addChild(std::move(scroll));
        for (size_t i = 0; i < m_pendingContents.size(); ++i) {
            m_tabContents[i] = m_pendingContents[i].get();
            m_pendingContents[i]->setVisible(i == m_currentTab);
            // Propagate theme to pending contents (added after applyThemeTo runs)
            if (m_theme) propagateTheme(m_pendingContents[i].get(), m_theme);
            m_contentHost->addChild(std::move(m_pendingContents[i]));
        }
    }

    m_tabBar->setBounds({0, 0, m_bounds.width, TAB_HEIGHT});
    m_tabBar->layout();

    float contentH = m_bounds.height - TAB_HEIGHT;
    if (contentH < 0) contentH = 0;
    m_scrollArea->setBounds({0, TAB_HEIGHT, m_bounds.width, contentH});

    // Restore saved scroll state for current tab BEFORE layout re-measures content
    if (m_currentTab < m_tabStates.size()) {
        m_scrollArea->setScrollX(m_tabStates[m_currentTab].scrollX);
        m_scrollArea->setScrollY(m_tabStates[m_currentTab].scrollY);
    }

    m_scrollArea->layout();

    // Clamp and save scroll state AFTER content is measured
    if (m_currentTab < m_tabStates.size()) {
        m_scrollArea->setScrollX(std::min(m_scrollArea->scrollX(), m_scrollArea->maxScrollX()));
        m_scrollArea->setScrollY(std::min(m_scrollArea->scrollY(), m_scrollArea->maxScrollY()));
        m_tabStates[m_currentTab].scrollX = m_scrollArea->scrollX();
        m_tabStates[m_currentTab].scrollY = m_scrollArea->scrollY();
    }
}

void TabView::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;

    // Determine colors (use theme if available, otherwise defaults)
    Color tabBarBg = m_hasTabBarColor ? m_tabBarColor : Color::fromHex(0xFF1A1A2E);
    Color tabActiveBg = Color::fromHex(0xFF2A2A3E);
    Color tabActiveIndicator = Color::fromHex(0xFF3B82F6);
    Color tabInactiveText = Color::fromHex(0xFF888888);

    // Try to use theme colors if set
    Theme* tm = m_theme;
    if (tm) {
        if (!m_hasTabBarColor) tabBarBg = tm->color(ColorRole::BgSecondary);
        tabActiveBg = tm->color(ColorRole::BgTertiary);
        tabActiveIndicator = tm->color(ColorRole::Primary);
        tabInactiveText = tm->color(ColorRole::TextDisabled);
    }

    // --- Draw tab bar ---
    painter.drawRect({sx, sy, m_bounds.width, TAB_HEIGHT}, tabBarBg);

    for (size_t i = 0; i < m_titles.size(); ++i) {
        Rect tr = tabRect(i);
        Rect r{sx + tr.x, sy + tr.y, tr.width, tr.height};
        Color bg = (i == m_currentTab) ? tabActiveBg : tabBarBg;
        painter.drawRect(r, bg);
        Color textCol = (i == m_currentTab) ? tabActiveIndicator : tabInactiveText;
        painter.drawText(r.insetBy(TAB_PADDING, 0), m_titles[i], textCol, TextAlign::Center);
    }

    // Active tab indicator line
    if (m_currentTab < m_titles.size()) {
        Rect tr = tabRect(m_currentTab);
        painter.drawRect({sx + tr.x, sy + tr.y + TAB_HEIGHT - 2, tr.width, 2}, tabActiveIndicator);
    }

    // --- Content area ---
    // Define clip rect for content (exactly below tab bar, within bounds)
    Rect contentClip{sx, sy + TAB_HEIGHT, m_bounds.width, m_bounds.height - TAB_HEIGHT};
    if (contentClip.width < 0) contentClip.width = 0;
    if (contentClip.height < 0) contentClip.height = 0;

    // Draw content border (inside clip area, at the edges)
    painter.pushClip(contentClip);
    if (m_hasContentBorder && m_contentBorderThickness > 0) {
        // Draw border around the content area
        painter.drawRectOutline(contentClip, m_contentBorderColor, m_contentBorderThickness);
    }

    if (m_scrollArea) m_scrollArea->paint(painter);
    painter.popClip();
}

bool TabView::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (e.localPos.y < TAB_HEIGHT) {
        for (size_t i = 0; i < m_titles.size(); ++i)
            if (tabRect(i).contains(e.localPos)) { switchTo(i); return true; }
        return true;
    }
    if (m_scrollArea && m_scrollArea->visible()) {
        MouseEvent ce = e; ce.localPos.y -= TAB_HEIGHT;
        return m_scrollArea->onMouseDown(ce);
    }
    return false;
}

bool TabView::onScrollWheel(float dx, float dy) {
    if (m_scrollArea) return m_scrollArea->onScrollWheel(dx, dy);
    return false;
}

Widget* TabView::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    // Clamp to bounds (including bottom edge)
    if (localPoint.x < 0 || localPoint.y < 0 ||
        localPoint.x >= m_bounds.width || localPoint.y >= m_bounds.height)
        return nullptr;
    if (localPoint.y < TAB_HEIGHT) return this;
    if (m_scrollArea && m_scrollArea->visible()) {
        Point childLocal{localPoint.x, localPoint.y - TAB_HEIGHT};
        return m_scrollArea->hitTest(childLocal);
    }
    return this;
}

} // namespace ui

#include "TabView.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"

namespace ui {

TabView::TabView() { m_focusable = false; }

size_t TabView::addTab(const std::string& title, std::unique_ptr<Widget> content) {
    m_titles.push_back(title);
    if (content) {
        content->setVisible(m_titles.size() == 1); // only first tab visible
        addChild(std::move(content)); // proper widget tree
    }
    return m_titles.size() - 1;
}

void TabView::removeTab(size_t index) {
    if (index >= m_titles.size()) return;
    m_titles.erase(m_titles.begin() + index);
    if (index < m_children.size()) {
        removeChild(m_children[index].get());
    }
    if (m_currentTab >= m_titles.size()) m_currentTab = m_titles.size() > 0 ? m_titles.size() - 1 : 0;
    switchTo(m_currentTab);
}

void TabView::setCurrentTab(size_t index) { switchTo(index); }

void TabView::setOnTabChanged(std::function<void(size_t)> cb) { m_onTabChanged = std::move(cb); }

Rect TabView::tabRect(size_t i) const {
    float x = 0;
    for (size_t j = 0; j < i; ++j) {
        x += m_titles[j].size() * 8.0f + TAB_PADDING * 2 + TAB_GAP;
    }
    float w = m_titles[i].size() * 8.0f + TAB_PADDING * 2;
    return {x, 0, w, TAB_HEIGHT};
}

void TabView::switchTo(size_t index) {
    if (index >= m_titles.size() || index == m_currentTab) return;
    if (m_currentTab < m_children.size()) {
        m_children[m_currentTab]->setVisible(false);
    }
    m_currentTab = index;
    if (m_currentTab < m_children.size()) {
        m_children[m_currentTab]->setVisible(true);
        if (m_bounds.width > 0) m_children[m_currentTab]->layout();
    }
    // Walk to root widget and mark layout dirty
    Widget* w = this;
    while (w->parent()) w = w->parent();
    w->markLayoutDirty();
    if (m_onTabChanged) m_onTabChanged(m_currentTab);
}

Size TabView::measure(const Size& available) const {
    float tabW = available.width > 0 ? available.width : 600.0f;
    for (size_t i = 0; i < m_titles.size(); ++i) {
        auto r = tabRect(i);
        if (r.right() > tabW) tabW = r.right();
    }
    float contentH = 0;
    if (m_currentTab < m_children.size() && m_children[m_currentTab]) {
        Size cs = m_children[m_currentTab]->measure({available.width, available.height - TAB_HEIGHT});
        contentH = cs.height;
    }
    return {tabW, TAB_HEIGHT + contentH};
}

void TabView::layout() {
    Rect contentArea{0, TAB_HEIGHT, m_bounds.width, m_bounds.height - TAB_HEIGHT};
    // Position and layout all children — bounds need to be set even for hidden tabs
    for (size_t i = 0; i < m_children.size(); ++i) {
        m_children[i]->setBounds(contentArea);
        m_children[i]->layout();
    }
}

void TabView::paint(Painter& painter) {
    float sx = screenRect().x, sy = screenRect().y;

    // Tab bar background
    painter.drawRect({sx, sy, m_bounds.width, TAB_HEIGHT}, Color::fromHex(0xFF1A1A2E));

    // Tabs
    for (size_t i = 0; i < m_titles.size(); ++i) {
        Rect tr = tabRect(i);
        Rect r{sx + tr.x, sy + tr.y, tr.width, tr.height};
        Color tabBg = (i == m_currentTab)
            ? Color::fromHex(0xFF2A2A3E)
            : Color::fromHex(0xFF1A1A2E);
        painter.drawRect(r, tabBg);

        Rect textR = r.insetBy(TAB_PADDING, 0);
        Color textCol = (i == m_currentTab)
            ? Color::fromHex(0xFF3B82F6)
            : Color::fromHex(0xFF888888);
        painter.drawText(textR, m_titles[i], textCol, TextAlign::Center);
    }

    // Active tab indicator
    if (m_currentTab < m_titles.size()) {
        Rect tr = tabRect(m_currentTab);
        painter.drawRect({sx + tr.x, sy + tr.y + TAB_HEIGHT - 2, tr.width, 2}, Color::fromHex(0xFF3B82F6));
    }

    // Paint visible content child
    for (size_t i = 0; i < m_children.size(); ++i) {
        if (m_children[i]->visible()) {
            m_children[i]->paint(painter);
            break;
        }
    }
}

bool TabView::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (e.localPos.y < TAB_HEIGHT) {
        for (size_t i = 0; i < m_titles.size(); ++i) {
            if (tabRect(i).contains(e.localPos)) {
                switchTo(i);
                return true;
            }
        }
        return true;
    }
    // Forward to content child
    if (m_currentTab < m_children.size()) {
        MouseEvent childEvent = e;
        childEvent.localPos.y -= TAB_HEIGHT;
        return m_children[m_currentTab]->onMouseDown(childEvent);
    }
    return false;
}

Widget* TabView::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x < 0 || localPoint.y < 0 || localPoint.x >= m_bounds.width) return nullptr;

    // Tab bar area: return self for tab clicks
    if (localPoint.y < TAB_HEIGHT) return this;

    // Content area: delegate to active child (which IS a proper child)
    if (m_currentTab < m_children.size() && m_children[m_currentTab]->visible()) {
        Point childLocal{localPoint.x, localPoint.y - TAB_HEIGHT};
        Widget* hit = m_children[m_currentTab]->hitTest(childLocal);
        if (hit) return hit;
    }
    return this;
}

} // namespace ui

#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Widget.hpp"

namespace ui {

class FileBrowser : public Widget {
public:
    FileBrowser();
    void setRootPath(const std::string& path);
    void setOnFileSelected(std::function<void(const std::string&)> cb) { m_onSelect = std::move(cb); }
    void setOnClose(std::function<void()> cb) { m_onClose = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }
    void setFilter(const std::string& ext);
    void show(Widget* parent, float w = 600, float h = 400);

    const char* typeName() const override { return "FileBrowser"; }
    Rect screenRect() const;
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseUp(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;
    bool onScrollWheel(float dx, float dy) override;
    Widget* hitTest(Point lp) override;

private:
    struct FileEntry { std::string name; bool isDir; };
    std::string m_rootPath, m_currentPath, m_filter;
    std::vector<FileEntry> m_entries;
    std::function<void(const std::string&)> m_onSelect;
    std::function<void()> m_onClose;
    class Font* m_font = nullptr;
    int m_selectedIdx = -1, m_hoveredIdx = -1;
    float m_sidebarW = 160;
    float m_w = 600, m_h = 400;
    Widget* m_parentWidget = nullptr;
    bool m_open = false, m_dragging = false, m_resizing = false;
    bool m_hoverClose = false, m_hoverResize = false;
    bool m_scrollDragging = false;
    float m_dragX = 0, m_dragY = 0, m_resizeStartW = 0, m_resizeStartH = 0;
    float m_scrollY = 0, m_scrollDragStartY = 0, m_scrollDragStartVal = 0;
    static constexpr float ROW_H = 24, TITLE_H = 32, SB_W = 8;
    void refresh();
    void navigateTo(const std::string& path);
    void goUp();
};

} // namespace ui

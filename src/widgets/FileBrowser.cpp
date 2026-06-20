#include "FileBrowser.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "core/Icon.hpp"
#include "res/Font.hpp"
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace ui {

FileBrowser::FileBrowser() { m_focusable = true; setRootPath("C:/"); }

void FileBrowser::setRootPath(const std::string& path) { m_rootPath = path; m_currentPath = path; refresh(); }
void FileBrowser::setFilter(const std::string& ext) { m_filter = ext; refresh(); }

void FileBrowser::navigateTo(const std::string& path) { m_currentPath = path; refresh(); }
void FileBrowser::goUp() {
    auto pos = m_currentPath.find_last_of("/\\");
    if (pos != std::string::npos && pos > 2) m_currentPath = m_currentPath.substr(0, pos);
    else m_currentPath = m_rootPath;
    refresh();
}

void FileBrowser::refresh() {
    m_entries.clear(); m_selectedIdx = -1; m_hoveredIdx = -1;
#ifdef _WIN32
    std::string search = m_currentPath + "\\*";
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(search.c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            std::string name = fd.cFileName; if (name == ".") continue;
            bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            if (!isDir && !m_filter.empty()) {
                std::string ext = name.substr(name.find_last_of('.'));
                if (m_filter.find(ext) == std::string::npos && !m_filter.empty()) continue;
            }
            m_entries.push_back({name, isDir});
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
#endif
    std::sort(m_entries.begin(), m_entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.isDir != b.isDir) return a.isDir > b.isDir; return a.name < b.name;
    });
}

void FileBrowser::show(Widget* parent, float w, float h) {
    m_parentWidget = parent; m_w = w; m_h = h; m_open = true; m_visible = true;
    if (parent) setBounds({(parent->bounds().width-w)*0.5f, (parent->bounds().height-h)*0.5f, w, h});
}

Rect FileBrowser::screenRect() const {
    if (m_parentWidget && m_open) return {0,0,m_parentWidget->bounds().width,m_parentWidget->bounds().height};
    return {globalPosition().x, globalPosition().y, m_w, m_h};
}
Widget* FileBrowser::hitTest(Point) { return m_open ? this : nullptr; }
Size FileBrowser::measure(const Size& a) const { (void)a; return {500,300}; }
void FileBrowser::layout() {}

void FileBrowser::paint(Painter& p) {
    Rect r = screenRect();
    float dx = (r.width-m_w)*0.5f, dy = (r.height-m_h)*0.5f;
    float lh = m_font ? m_font->lineHeight() : 18;

    // Backdrop
    p.drawRect({0,0,r.width,r.height}, Color{0,0,0,0.55f});

    // Dialog card
    Rect dlg{dx, dy, m_w, m_h};
    p.drawRoundedRect(dlg, Color::fromHex(0xFF1E1E2E), 10);
    p.drawRectOutline(dlg, Color::fromHex(0xFF444466), 1);

    // --- Title bar ---
    p.drawRect({dx, dy, m_w, TITLE_H}, Color::fromHex(0xFF252535));
    // Back button
    Rect backBtn{dx+4, dy+4, 24, 24};
    p.drawRoundedRect(backBtn, Color::fromHex(0xFF3A3A4A), 4);
    drawIcon(p, IconType::ArrowLeft, dx+8, dy+8, 16, Color{0.7f,0.7f,0.8f,1});
    // Up button
    Rect upBtn{dx+32, dy+4, 24, 24};
    p.drawRoundedRect(upBtn, Color::fromHex(0xFF3A3A4A), 4);
    p.drawText(upBtn, "^", Color{0.7f,0.7f,0.8f,1}, TextAlign::Center);
    // Path text
    p.drawText({dx+64, dy+6, m_w-120, lh}, m_currentPath, Color{0.75f,0.75f,0.75f,1}, TextAlign::Left);

    // Close X
    float cx = dx + m_w - 28, cy = dy + 6;
    Rect closeBtn{cx, cy, 20, 20};
    Color xBg = m_hoverClose ? Color{0.9f,0.2f,0.2f,1} : Color::fromHex(0xFF3A3A4A);
    p.drawRoundedRect(closeBtn, xBg, 4);
    p.drawText(closeBtn, "X", Color{1,1,1,1}, TextAlign::Center);

    // --- Sidebar ---
    float sx = dx, sy = dy + TITLE_H;
    p.drawRect({sx, sy, m_sidebarW, m_h-TITLE_H}, Color::fromHex(0xFF1C1C28));
    p.drawRect({sx+m_sidebarW, sy, 1, m_h-TITLE_H}, Color::fromHex(0xFF333344));
    struct Place { const char* name; const char* path; };
    Place places[] = {{"Desktop", "C:/Users"}, {"Documents", "C:/Users"}, {"Downloads", "C:/Users"}, {"This PC", "C:/"}};
    for (int i = 0; i < 4; ++i) {
        Color pc = (m_hoveredIdx == -100 - i) ? Color{0.3f,0.3f,0.45f,1} : Color{0,0,0,0};
        if (pc.a > 0) p.drawRect({sx, sy+8+i*lh, m_sidebarW, lh}, pc);
        drawIcon(p, IconType::Folder, sx+10, sy+10+i*lh, 14, Color{0.4f,0.6f,1,1});
        p.drawText({sx+30, sy+8+i*lh, m_sidebarW-36, lh}, places[i].name, Color{0.7f,0.7f,0.8f,1}, TextAlign::Left);
    }

    // Filter bar
    float mx = sx + m_sidebarW + 1, my = sy;
    p.drawRect({mx, my, m_w-m_sidebarW-1, lh+4}, Color::fromHex(0xFF222233));
    std::string filterInfo = m_filter.empty() ? "All Files" : ("Filter: " + m_filter);
    p.drawText({mx+8, my+2, m_w-m_sidebarW-20, lh}, filterInfo, Color{0.5f,0.5f,0.5f,1}, TextAlign::Left);

    // File rows with scroll
    float y = my + lh + 6;
    float visibleH = m_h - TITLE_H - lh - 28;
    float maxRows = visibleH / ROW_H;
    float totalH = m_entries.size() * ROW_H;
    float maxScroll = std::max(0.0f, totalH - visibleH);
    if (m_scrollY > maxScroll) m_scrollY = maxScroll;
    if (m_scrollY < 0) m_scrollY = 0;
    int startIdx = (int)(m_scrollY / ROW_H);
    for (size_t i = (size_t)startIdx; i < m_entries.size() && (int)(i-startIdx) < (int)maxRows; ++i) {
        float ry = y + (i - startIdx) * ROW_H;
        Color rowBg{0,0,0,0};
        if ((int)i == m_selectedIdx) rowBg = Color{0.2f,0.4f,0.8f,0.5f};
        else if ((int)i == m_hoveredIdx) rowBg = Color{0.3f,0.3f,0.4f,0.4f};
        if (rowBg.a > 0) p.drawRect({mx, ry, m_w-m_sidebarW-1, ROW_H}, rowBg);
        drawIcon(p, m_entries[i].isDir ? IconType::Folder : IconType::File, mx+8, ry+4, 16,
                 m_entries[i].isDir ? Color{0.4f,0.6f,1,1} : Color{0.6f,0.6f,0.6f,1});
        p.drawText({mx+30, ry+2, m_w-m_sidebarW-50, lh}, m_entries[i].name,
                   (int)i == m_selectedIdx ? Color{1,1,1,1} : Color{0.8f,0.8f,0.8f,1}, TextAlign::Left);
    }

    // Bottom bar
    float by = dy + m_h - 22;
    p.drawRect({dx, by, m_w, 22}, Color::fromHex(0xFF1C1C28));
    p.drawText({dx+8, by+2, 200, 16}, std::to_string(m_entries.size())+" items", Color{0.5f,0.5f,0.5f,1}, TextAlign::Left);

    // Resize handle
    float rrx = dx + m_w - 16, rry = dy + m_h - 16;
    Color rsCol = m_hoverResize ? Color{0.5f,0.5f,0.7f,1} : Color{0.35f,0.35f,0.4f,1};
    p.drawText({rrx-4, rry-4, 16, 16}, "::", rsCol, TextAlign::Center);

    // Scrollbar
    if (maxScroll > 0) {
        float sbX = dx + m_w - SB_W, sbY = dy + TITLE_H + lh + 4, sbH = visibleH;
        p.drawRect({sbX, sbY, SB_W-1, sbH}, Color::fromHex(0xFF2A2A3A));
        float thumbH = std::max(24.0f, sbH * (visibleH / totalH));
        float thumbY = sbY + (sbH - thumbH) * (m_scrollY / maxScroll);
        Color thumbCol = m_scrollDragging ? Color::fromHex(0xFF888899) : Color::fromHex(0xFF555566);
        p.drawRoundedRect({sbX, thumbY, SB_W-1, thumbH}, thumbCol, 3);
    }
}

bool FileBrowser::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    float lh = m_font ? m_font->lineHeight() : 18;
    float dx = (m_parentWidget ? (m_parentWidget->bounds().width - m_w)*0.5f : 0);
    float dy = (m_parentWidget ? (m_parentWidget->bounds().height - m_h)*0.5f : 0);
    float lx = e.localPos.x - dx, ly = e.localPos.y - dy;

    // Close X
    if (lx >= m_w - 30 && lx <= m_w - 8 && ly >= 4 && ly <= 26) { m_open = false; m_visible = false; if (m_onClose) m_onClose(); return true; }
    // Back button
    if (lx >= 4 && lx <= 28 && ly >= 4 && ly <= 28) { goUp(); return true; }
    // Up button
    if (lx >= 32 && lx <= 56 && ly >= 4 && ly <= 28) { goUp(); return true; }
    // Scrollbar hit test
    float sbX = m_w - SB_W;
    float visibleH = m_h - TITLE_H - lh - 28;
    float totalH = m_entries.size() * ROW_H;
    float maxScroll = std::max(0.0f, totalH - visibleH);
    if (maxScroll > 0 && lx >= sbX && ly >= TITLE_H + lh + 4 && ly <= TITLE_H + lh + 4 + visibleH) {
        float sbY = TITLE_H + lh + 4;
        float thumbH = std::max(24.0f, visibleH * (visibleH / totalH));
        float thumbY = sbY + (visibleH - thumbH) * (m_scrollY / maxScroll);
        if (ly >= thumbY && ly <= thumbY + thumbH) {
            m_scrollDragging = true; m_scrollDragStartY = ly; m_scrollDragStartVal = m_scrollY; return true;
        } else {
            m_scrollY = (ly - sbY - thumbH/2) / (visibleH - thumbH) * maxScroll;
            m_scrollY = std::clamp(m_scrollY, 0.0f, maxScroll); return true;
        }
    }
    // Resize corner (only if within handle area)
    if (lx >= m_w - 20 && ly >= m_h - 20) { m_resizing = true; m_resizeStartW = m_w; m_resizeStartH = m_h; m_dragX = e.localPos.x; m_dragY = e.localPos.y; return true; }
    // Title drag
    if (ly < TITLE_H && lx < m_w - 30) { m_dragging = true; m_dragX = lx; m_dragY = ly; return true; }
    // Outside dialog
    if (lx < 0 || ly < 0 || lx >= m_w || ly >= m_h) return true;

    // Sidebar
    if (lx < m_sidebarW) {
        const char* paths[] = {"C:/Users", "C:/Users", "C:/Users", "C:/"};
        int idx = (int)((ly - TITLE_H - 8) / lh);
        if (idx >= 0 && idx < 4) navigateTo(paths[idx]);
        return true;
    }
    // File list
    float mainX = m_sidebarW + 1;
    float fy = ly - TITLE_H - lh - 6 + m_scrollY;
    int idx = (int)(fy / ROW_H);
    if (idx >= 0 && idx < (int)m_entries.size()) {
        m_selectedIdx = idx;
        if (m_entries[idx].isDir) { navigateTo(m_currentPath + "/" + m_entries[idx].name); }
        else if (m_onSelect) { m_onSelect(m_currentPath + "/" + m_entries[idx].name); }
    }
    return true;
}

bool FileBrowser::onMouseUp(MouseEvent&) { m_scrollDragging = false; m_dragging = false; m_resizing = false; return true; }

bool FileBrowser::onScrollWheel(float, float dy) {
    float lh = m_font ? m_font->lineHeight() : 18;
    float visibleH = m_h - TITLE_H - lh - 28;
    float totalH = m_entries.size() * ROW_H;
    float maxScroll = std::max(0.0f, totalH - visibleH);
    m_scrollY = std::clamp(m_scrollY - dy * 20, 0.0f, maxScroll);
    return true;
}

bool FileBrowser::onMouseMove(MouseEvent& e) {
    float lh = m_font ? m_font->lineHeight() : 18;
    float dx = (m_parentWidget ? (m_parentWidget->bounds().width - m_w)*0.5f : 0);
    float dy = (m_parentWidget ? (m_parentWidget->bounds().height - m_h)*0.5f : 0);
    float lx = e.localPos.x - dx, ly = e.localPos.y - dy;

    m_hoverClose = (lx >= m_w - 30 && lx <= m_w - 8 && ly >= 4 && ly <= 26);
    m_hoverResize = (lx >= m_w - 20 && ly >= m_h - 20);

    if (m_dragging) {
        float nx = e.localPos.x - m_dragX, ny = e.localPos.y - m_dragY;
        if (m_parentWidget) setBounds({nx, ny, m_w, m_h});
        return true;
    }
    if (m_scrollDragging) {
        float sbY = TITLE_H + lh + 4;
        float visibleH = m_h - TITLE_H - lh - 28;
        float totalH = m_entries.size() * ROW_H;
        float maxScroll = std::max(0.0f, totalH - visibleH);
        float thumbH = std::max(24.0f, visibleH * (visibleH / totalH));
        float dy = ly - m_scrollDragStartY;
        float scrollRange = visibleH - thumbH;
        if (scrollRange > 0) m_scrollY = std::clamp(m_scrollDragStartVal + dy * maxScroll / scrollRange, 0.0f, maxScroll);
        return true;
    }
    if (m_resizing) {
        m_w = std::max(300.0f, m_resizeStartW + e.localPos.x - m_dragX);
        m_h = std::max(200.0f, m_resizeStartH + e.localPos.y - m_dragY);
        if (m_parentWidget) setBounds({bounds().x, bounds().y, m_w, m_h});
        return true;
    }

    // Sidebar hover
    if (lx >= 0 && lx < m_sidebarW && ly >= TITLE_H) {
        int idx = (int)((ly - TITLE_H - 8) / lh);
        m_hoveredIdx = (idx >= 0 && idx < 4) ? -100 - idx : -1;
        return true;
    }
    // File list hover
    float mainX = m_sidebarW + 1;
    if (lx >= mainX && ly >= TITLE_H + lh + 4) {
        float fy = ly - TITLE_H - lh - 6 + m_scrollY;
        int idx = (int)(fy / ROW_H);
        m_hoveredIdx = (idx >= 0 && idx < (int)m_entries.size()) ? idx : -1;
    } else m_hoveredIdx = -1;
    return true;
}

} // namespace ui

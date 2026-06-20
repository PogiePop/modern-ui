#include "Upload.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "core/Icon.hpp"

namespace ui {
Upload::Upload() { m_focusable = true; m_tabIndex = 0; }

void Upload::paint(Painter& p) {
    Rect r = screenRect();
    Color borderCol = m_hovered ? Color{0.4f,0.6f,1,1} : Color{0.3f,0.3f,0.4f,1};
    if (!m_selectedFile.empty()) borderCol = Color{0.2f,0.7f,0.3f,1};
    p.drawRectOutline(r, borderCol, 2);
    IconType ic = m_selectedFile.empty() ? IconType::Upload : IconType::Check;
    drawIcon(p, ic, r.x+r.width*0.5f-14, r.y+8, 28, borderCol);
    std::string display = m_selectedFile.empty() ? m_text : m_selectedFile;
    if (!m_selectedFile.empty()) {
        auto pos = m_selectedFile.find_last_of("/\\");
        display = (pos != std::string::npos) ? m_selectedFile.substr(pos+1) : m_selectedFile;
    }
    p.drawText({r.x+8, r.y+r.height-24, r.width-16, 20}, display, Color{0.8f,0.8f,0.8f,1}, TextAlign::Center);
}

bool Upload::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (m_openPicker) m_openPicker(this);
    return true;
}
} // namespace ui

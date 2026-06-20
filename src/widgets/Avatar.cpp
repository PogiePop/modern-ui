#include "Avatar.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"
#include <cctype>

namespace ui {

Avatar::Avatar() { m_focusable = false; }

void Avatar::setName(const std::string& name) {
    m_initials.clear();
    if (!name.empty()) {
        m_initials += (char)toupper(name[0]);
        size_t sp = name.find(' ');
        if (sp != std::string::npos && sp+1 < name.size())
            m_initials += (char)toupper(name[sp+1]);
    }
}

void Avatar::setInitials(const std::string& init) { m_initials = init.substr(0, 2); }
void Avatar::setSize(float d) { m_diameter = d; m_minWidth = d; m_minHeight = d; }
void Avatar::setColor(const Color& bg) { m_bgColor = bg; }

void Avatar::paint(Painter& painter) {
    Rect r = screenRect();
    float cx = r.x + r.width*0.5f, cy = r.y + r.height*0.5f, radius = r.width*0.5f;
    // Circular background
    painter.drawRoundedRect({r.x, r.y, m_diameter, m_diameter}, m_bgColor, radius);
    // Initials text centered
    if (!m_initials.empty()) {
        Rect tr{r.x, r.y, m_diameter, m_diameter};
        float fs = m_font ? m_font->measureText(m_initials) : m_initials.size()*10.0f;
        (void)fs;
        painter.drawText({r.x, r.y, m_diameter, m_diameter}, m_initials, m_textColor, TextAlign::Center);
    }
}

} // namespace ui

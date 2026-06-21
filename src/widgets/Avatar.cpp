#include "Avatar.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
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

void Avatar::paint(Painter& painter) {
    Rect r = screenRect();
    float cx = r.x + r.width*0.5f, cy = r.y + r.height*0.5f, radius = r.width*0.5f;

    // Resolve bg color: custom > theme role > theme Primary > default
    Color bg = m_bgColor;
    if (!m_useCustomColor && m_theme) {
        bg = m_useColorRole ? m_theme->color(m_colorRole) : m_theme->color(ColorRole::Primary);
    }

    painter.drawRoundedRect({r.x, r.y, m_diameter, m_diameter}, bg, radius);
    if (!m_initials.empty()) {
        painter.drawText({r.x, r.y, m_diameter, m_diameter}, m_initials, m_textColor, TextAlign::Center);
    }
}

} // namespace ui

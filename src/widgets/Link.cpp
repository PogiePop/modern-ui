#include "Link.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Font.hpp"
#include "utils/Navigation.hpp"
#include "utils/HttpClient.hpp"
#include "utils/AsyncTask.hpp"
#include <cstdio>

namespace ui {
Link::Link(const std::string& text) : m_text(text) { m_focusable = true; m_tabIndex = 0; }

Size Link::measure(const Size&) const {
    float w = m_font ? m_font->measureText(m_text) : m_text.size()*8.0f;
    float h = m_font ? m_font->lineHeight() : 16;
    return {w, h};
}

void Link::paint(Painter& p) {
    Rect r = screenRect();
    Color c = m_hovered ? Color{0.4f,0.6f,1,1} : Color{0.3f,0.5f,0.9f,1};
    p.drawText(r, m_text, c, TextAlign::Left);
    p.drawRect({r.x, r.y+r.height-1, r.width, 1}, c);
}

bool Link::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (m_onClick) { m_onClick(); return true; }
    if (!m_url.empty()) {
        if (m_mode == Navigate) {
            Navigation::openUrl(m_url);
        } else {
            HttpClient::getAsync(m_url, [](const HttpResponse& r) {
                printf("[Link] HTTP GET %d: %s\n", r.statusCode, r.body.substr(0, 200).c_str());
            });
        }
        return true;
    }
    return false;
}
} // namespace ui

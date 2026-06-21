#include "DatePicker.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"
#include "res/Font.hpp"
#include <cstdio>
#include <ctime>

namespace ui {

DatePicker::DatePicker() : Container(LayoutDirection::Vertical) {
    m_focusable = true;
    setPadding({4, 8, 8, 8});
    setSpacing(4);
    // Default to today
    time_t now = time(nullptr);
    struct tm* tm = localtime(&now);
    m_year = tm->tm_year + 1900;
    m_month = tm->tm_mon + 1;
    m_day = tm->tm_mday;
}

int DatePicker::daysInMonth(int y, int m) const {
    static const int d[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int days = d[m-1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) days = 29;
    return days;
}

int DatePicker::firstWeekday(int y, int m) const {
    // Zeller-like: 2026-01-01 is a Thursday (4)
    if (m < 3) { m += 12; y--; }
    int k = y % 100, j = y / 100;
    int h = (1 + 13*(m+1)/5 + k + k/4 + j/4 + 5*j) % 7;
    return (h + 6) % 7; // 0=Sun
}

void DatePicker::setDate(const std::string& iso) {
    int y, m, d;
    if (sscanf(iso.c_str(), "%d-%d-%d", &y, &m, &d) == 3) setDate(y, m, d);
}

std::string DatePicker::date() const {
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", m_year, m_month, m_day);
    return buf;
}

void DatePicker::setDate(int year, int month, int day) {
    m_year = year;
    m_month = month < 1 ? 1 : (month > 12 ? 12 : month);
    int maxDay = daysInMonth(m_year, m_month);
    m_day = day < 1 ? 1 : (day > maxDay ? maxDay : day);
}

void DatePicker::fireChanged() {
    if (m_onChanged) m_onChanged(m_year, m_month, m_day);
}

Size DatePicker::measure(const Size& a) const {
    float w = CELL_W * 7 + 8;
    float h = HEADER_H + 8 + CELL_H * 7 + 16;
    return {w, h};
}

void DatePicker::layout() {}

void DatePicker::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 16;

    // Background
    Color bg = Color::fromHex(0xFF1E1E2E);
    Color textCol = Color::fromHex(0xFFE4E4EC);
    Color accentCol = Color::fromHex(0xFF4F8CF7);
    Color dimCol = Color::fromHex(0xFF5A5A72);
    Color arrowCol = Color::fromHex(0xFF8888A0);
    if (m_theme) {
        bg = m_theme->color(ColorRole::Surface);
        textCol = m_theme->color(ColorRole::Text);
        accentCol = m_theme->color(ColorRole::Primary);
        dimCol = m_theme->color(ColorRole::TextDisabled);
        arrowCol = m_theme->color(ColorRole::TextSecondary);
    }
    p.drawRoundedRect(r, bg, 8);

    // Header: <  |  YEAR MONTH  |  >
    // Three separate zones for clear click targets
    float hy = r.y + 4;
    float arrowW = 32; // fixed width for each arrow zone
    float leftArrowX = r.x + 4;
    float rightArrowX = r.x + r.width - arrowW - 4;
    float centerX = r.x + arrowW + 4;
    float centerW = r.width - 2 * arrowW - 8;

    // Left arrow (prev month)
    p.drawText({leftArrowX, hy, arrowW, HEADER_H}, "<", arrowCol, TextAlign::Center);

    // Month/Year label (center)
    static const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char hdr[32];
    snprintf(hdr, sizeof(hdr), "%d %s", m_year, months[m_month-1]);
    p.drawText({centerX, hy, centerW, HEADER_H}, hdr, textCol, TextAlign::Center);

    // Right arrow (next month)
    p.drawText({rightArrowX, hy, arrowW, HEADER_H}, ">", arrowCol, TextAlign::Center);

    // Weekday headers
    static const char* wdays[] = {"Su","Mo","Tu","We","Th","Fr","Sa"};
    float gy = r.y + HEADER_H + 6;
    for (int i = 0; i < 7; ++i) {
        p.drawText({r.x + 4 + i * CELL_W, gy, CELL_W, lh}, wdays[i], dimCol, TextAlign::Center);
    }

    // Day grid
    int fd = firstWeekday(m_year, m_month);
    int dim = daysInMonth(m_year, m_month);
    float dy = gy + lh + 2;
    for (int i = 0; i < dim; ++i) {
        int col = (fd + i) % 7;
        int row = (fd + i) / 7;
        float cx = r.x + 4 + col * CELL_W;
        float cy = dy + row * CELL_H;
        bool selected = (i + 1 == m_day);
        if (selected) {
            p.drawRoundedRect({cx + 1, cy, CELL_W - 2, CELL_H}, accentCol, 4);
            p.drawText({cx, cy, CELL_W, CELL_H}, std::to_string(i + 1), Color{1,1,1,1}, TextAlign::Center);
        } else {
            p.drawText({cx, cy, CELL_W, CELL_H}, std::to_string(i + 1), textCol, TextAlign::Center);
        }
    }
}

bool DatePicker::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    float lh = m_font ? m_font->lineHeight() : 16;

    // Header Y range: [4, HEADER_H + 4)
    float hdrTop = 4;
    float hdrBottom = HEADER_H + 4;

    if (e.localPos.y >= hdrTop && e.localPos.y < hdrBottom) {
        float arrowW = 32;
        // Left arrow zone: [4, 4 + arrowW)
        if (e.localPos.x >= 4 && e.localPos.x < 4 + arrowW) {
            m_month--; if (m_month < 1) { m_month = 12; m_year--; }
            int maxD = daysInMonth(m_year, m_month);
            if (m_day > maxD) m_day = maxD;
            fireChanged(); return true;
        }
        // Right arrow zone: [width - arrowW - 4, width - 4)
        float rightStart = m_bounds.width - arrowW - 4;
        if (e.localPos.x >= rightStart && e.localPos.x < m_bounds.width - 4) {
            m_month++; if (m_month > 12) { m_month = 1; m_year++; }
            int maxD = daysInMonth(m_year, m_month);
            if (m_day > maxD) m_day = maxD;
            fireChanged(); return true;
        }
        // Click on center label — ignored (not prev/next)
        return true;
    }

    // Day grid click
    float gy = HEADER_H + 6 + lh + 2;
    int fd = firstWeekday(m_year, m_month);
    float relY = e.localPos.y - gy;
    float relX = e.localPos.x - 4;
    if (relY < 0) return false;
    int col = (int)(relX / CELL_W);
    int row = (int)(relY / CELL_H);
    if (col < 0 || col > 6) return false;
    int idx = row * 7 + col - fd;
    if (idx >= 0 && idx < daysInMonth(m_year, m_month)) {
        m_day = idx + 1;
        fireChanged(); return true;
    }
    return false;
}

} // namespace ui

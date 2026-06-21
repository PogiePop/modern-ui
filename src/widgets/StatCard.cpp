#include "StatCard.hpp"
#include "core/Painter.hpp"
#include "res/Theme.hpp"
#include "res/Font.hpp"

namespace ui {

// ============================================================
// StatCard
// ============================================================
StatCard::StatCard() { m_focusable = false; }

Size StatCard::measure(const Size&) const {
    float h = 20 + 28 + 16; // label + value + trend
    return {140, h};
}

void StatCard::paint(Painter& p) {
    Rect r = screenRect();

    // Background
    Color bg = Color::fromHex(0xFF1E1E2E);
    if (m_theme) bg = m_theme->color(ColorRole::Surface);
    p.drawRoundedRect(r, bg, 8);

    // Accent color from variant or theme
    if (!m_customAccent && m_theme) {
        switch (m_variant) {
        case Primary: m_accentColor = m_theme->color(ColorRole::Primary); break;
        case Success: m_accentColor = m_theme->color(ColorRole::Success); break;
        case Danger:  m_accentColor = m_theme->color(ColorRole::Danger);  break;
        case Warning: m_accentColor = m_theme->color(ColorRole::Warning); break;
        case Accent:  m_accentColor = m_theme->color(ColorRole::Accent);  break;
        case Info:    m_accentColor = m_theme->color(ColorRole::Info);    break;
        }
    }
    // Size variant
    float labelSize = 12, valueSize = 28;
    if (m_sizeVariant == Small) { labelSize = 10; valueSize = 22; }
    else if (m_sizeVariant == Large) { labelSize = 14; valueSize = 34; }

    // Label
    float lh = m_font ? m_font->lineHeight() : 16;
    Color labelCol = Color{0.6f, 0.6f, 0.7f, 1};
    if (m_theme) labelCol = m_theme->color(ColorRole::TextSecondary);
    p.drawText({r.x + 12, r.y + 8, r.width - 24, lh}, m_label, labelCol, TextAlign::Left);

    // Value
    Color valCol = Color{1, 1, 1, 1};
    if (m_theme) valCol = m_theme->color(ColorRole::Text);
    p.drawText({r.x + 12, r.y + 28, r.width - 24, 28}, m_value, valCol, TextAlign::Left);

    // Trend
    if (!m_trend.empty()) {
        Color trendCol = m_trendPositive ? Color{0.2f, 0.8f, 0.4f, 1} : Color{1, 0.3f, 0.3f, 1};
        if (m_theme) trendCol = m_trendPositive ? m_theme->color(ColorRole::Success) : m_theme->color(ColorRole::Danger);
        std::string t = (m_trendPositive ? "+" : "") + m_trend;
        p.drawText({r.x + 12, r.y + 54, r.width - 24, lh}, t, trendCol, TextAlign::Left);
    }
}

// ============================================================
// BarChart
// ============================================================
BarChart::BarChart() { m_focusable = false; }

void BarChart::setData(const std::vector<float>& values, const std::vector<std::string>& labels) {
    m_values = values; m_labels = labels;
}

Size BarChart::measure(const Size&) const { return {300, 150}; }

void BarChart::paint(Painter& p) {
    Rect r = screenRect();
    if (m_values.empty()) return;

    // Background
    Color bg = Color::fromHex(0xFF1E1E2E);
    if (m_theme) bg = m_theme->color(ColorRole::Surface);
    p.drawRoundedRect(r, bg, 8);

    if (m_theme && m_useColorRole) m_barColor = m_theme->color(m_barColorRole);
    else if (!m_customColor && m_theme) m_barColor = m_theme->color(ColorRole::Primary);

    float maxVal = 1.0f;
    for (float v : m_values) if (v > maxVal) maxVal = v;

    float chartX = r.x + 8, chartY = r.y + 24;
    float chartW = r.width - 16, chartH = r.height - 40;
    float gap = 8.0f;
    float barW = (chartW - gap * (m_values.size() - 1)) / m_values.size();
    if (barW < 4) barW = 4;

    for (size_t i = 0; i < m_values.size(); ++i) {
        float bh = (m_values[i] / maxVal) * chartH;
        if (bh < 2) bh = 2;
        float bx = chartX + i * (barW + gap);
        float by = chartY + chartH - bh;
        p.drawRoundedRect({bx, by, barW, bh}, m_barColor, 2);

        // Label
        if (i < m_labels.size() && !m_labels[i].empty()) {
            float lh = m_font ? m_font->lineHeight() : 14;
            p.drawText({bx, r.y + r.height - 16, barW, lh}, m_labels[i],
                       Color{0.6f, 0.6f, 0.7f, 1}, TextAlign::Center);
        }
    }
}

} // namespace ui

#pragma once
#include <string>
#include "Widget.hpp"
#include "res/Theme.hpp"

namespace ui {

// Statistics card with variant colors and sizes
class StatCard : public Widget {
public:
    enum Variant { Primary, Success, Danger, Warning, Accent, Info };
    enum SizeVariant { Small, Default, Large };

    StatCard();

    void setLabel(const std::string& text) { m_label = text; }
    void setValue(const std::string& val) { m_value = val; }
    void setTrend(const std::string& t, bool positive = true) { m_trend = t; m_trendPositive = positive; }
    void setColor(const Color& accent) { m_accentColor = accent; m_customAccent = true; }
    void setVariant(Variant v) { m_variant = v; m_customAccent = false; }
    void setSizeVariant(SizeVariant sv) { m_sizeVariant = sv; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "StatCard"; }
    Size measure(const Size&) const override;
    void paint(Painter& p) override;

private:
    std::string m_label, m_value, m_trend;
    bool m_trendPositive = true;
    Color m_accentColor{0.3f, 0.5f, 1, 1};
    bool m_customAccent = false;
    Variant m_variant = Primary;
    SizeVariant m_sizeVariant = Default;
    class Font* m_font = nullptr;
};

// Bar chart with variant colors
class BarChart : public Widget {
public:
    BarChart();

    void setData(const std::vector<float>& values, const std::vector<std::string>& labels = {});
    void setBarColor(const Color& c) { m_barColor = c; m_customColor = true; }
    void setColorRole(ColorRole role) { m_barColorRole = role; m_useColorRole = true; m_customColor = false; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "BarChart"; }
    Size measure(const Size&) const override;
    void paint(Painter& p) override;

private:
    std::vector<float> m_values;
    std::vector<std::string> m_labels;
    Color m_barColor{0.3f, 0.5f, 1, 1};
    ColorRole m_barColorRole = ColorRole::Primary;
    bool m_customColor = false, m_useColorRole = false;
    class Font* m_font = nullptr;
};

} // namespace ui

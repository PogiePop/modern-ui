#pragma once

#include <array>
#include <string>

#include "core/Types.hpp"

namespace ui {

// --- Token enums ---

enum class ColorRole {
    Bg, BgSecondary, BgTertiary,
    Text, TextSecondary, TextDisabled,
    Primary, PrimaryHover, PrimaryActive,
    Border, BorderFocused,
    Accent, AccentHover,
    Transparent,
    _Count
};

enum class FontSizeRole {
    Caption, Body, Subheading, Heading, Title,
    _Count
};

enum class SpacingRole {
    Xs, Sm, Md, Lg, Xl,
    _Count
};

enum class BorderRadiusRole {
    None, Sm, Md, Lg,
    _Count
};

// --- Theme class ---

class Theme {
public:
    Theme();

    static Theme defaultLight();
    static Theme defaultDark();

    // Setters
    void setColor(ColorRole role, Color color);
    void setFontSize(FontSizeRole role, float size);
    void setSpacing(SpacingRole role, float value);
    void setBorderRadius(BorderRadiusRole role, float value);
    void setFontFamily(const char* path);

    // Getters
    Color color(ColorRole role) const;
    float fontSize(FontSizeRole role) const;
    float spacing(SpacingRole role) const;
    float borderRadius(BorderRadiusRole role) const;
    const std::string& fontFamily() const { return m_fontFamily; }

private:
    std::array<Color, static_cast<size_t>(ColorRole::_Count)> m_colors;
    std::array<float, static_cast<size_t>(FontSizeRole::_Count)> m_fontSizes;
    std::array<float, static_cast<size_t>(SpacingRole::_Count)> m_spacings;
    std::array<float, static_cast<size_t>(BorderRadiusRole::_Count)> m_borderRadii;
    std::string m_fontFamily;
};

} // namespace ui

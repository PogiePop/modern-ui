#pragma once

#include <array>
#include <string>

#include "core/Types.hpp"

namespace ui {

// --- Token enums ---

enum class ColorRole {
    // Backgrounds
    Bg, BgSecondary, BgTertiary,
    // Surfaces (cards, panels, elevated areas)
    Surface, SurfaceHover, SurfaceActive,
    // Text
    Text, TextSecondary, TextDisabled,
    // Primary brand
    Primary, PrimaryHover, PrimaryActive,
    // Borders
    Border, BorderFocused,
    // Accent (secondary brand)
    Accent, AccentHover,
    // Semantic
    Success, SuccessHover,
    Danger, DangerHover,
    Warning, WarningHover,
    Info, InfoHover,
    // Special
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
    Theme(); // default = dark theme (safe — doesn't recurse)

    static Theme defaultLight();
    static Theme defaultDark();

private:
    struct NoInit {};
public:
    explicit Theme(NoInit) {} // blank theme for factory methods (public for ThemeTransition)

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

    friend class ThemeTransition;
};

// Smooth theme transition — interpolates all colors over time
class ThemeTransition {
public:
    ThemeTransition();

    // Set the target theme (starts animating from current interpolated state)
    void setTarget(const Theme& theme, float duration = 0.4f);
    // Snap immediately to a theme (no animation)
    void setImmediate(const Theme& theme);

    // Call each frame — advances the interpolation
    void tick(float dt);

    bool isAnimating() const { return m_elapsed < m_duration; }
    const Theme& current() const { return m_current; }

private:
    Theme m_current;   // interpolated result (what widgets see)
    Theme m_from;      // snapshot at transition start
    Theme m_target;    // desired final theme
    float m_elapsed = 1.0f;
    float m_duration = 0.4f;
};

} // namespace ui

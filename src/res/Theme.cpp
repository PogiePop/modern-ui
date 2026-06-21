#include "Theme.hpp"

namespace ui {

// Default = dark theme
Theme::Theme() {
    // Backgrounds
    setColor(ColorRole::Bg,                Color::fromHex(0xFF0F0F16));
    setColor(ColorRole::BgSecondary,       Color::fromHex(0xFF1A1A26));
    setColor(ColorRole::BgTertiary,        Color::fromHex(0xFF242438));
    // Surfaces
    setColor(ColorRole::Surface,           Color::fromHex(0xFF1E1E2E));
    setColor(ColorRole::SurfaceHover,      Color::fromHex(0xFF28283C));
    setColor(ColorRole::SurfaceActive,     Color::fromHex(0xFF32324A));
    // Text
    setColor(ColorRole::Text,              Color::fromHex(0xFFE4E4EC));
    setColor(ColorRole::TextSecondary,     Color::fromHex(0xFF8888A0));
    setColor(ColorRole::TextDisabled,      Color::fromHex(0xFF5A5A72));
    // Primary
    setColor(ColorRole::Primary,           Color::fromHex(0xFF4F8CF7));
    setColor(ColorRole::PrimaryHover,      Color::fromHex(0xFF669DF8));
    setColor(ColorRole::PrimaryActive,     Color::fromHex(0xFF3B7AE6));
    // Borders
    setColor(ColorRole::Border,            Color::fromHex(0xFF2E2E3E));
    setColor(ColorRole::BorderFocused,     Color::fromHex(0xFF4F8CF7));
    // Accent
    setColor(ColorRole::Accent,            Color::fromHex(0xFFA78BFA));
    setColor(ColorRole::AccentHover,       Color::fromHex(0xFFC4B5FD));
    // Semantic
    setColor(ColorRole::Success,           Color::fromHex(0xFF34D399));
    setColor(ColorRole::SuccessHover,      Color::fromHex(0xFF6EE7B7));
    setColor(ColorRole::Danger,            Color::fromHex(0xFFF87171));
    setColor(ColorRole::DangerHover,       Color::fromHex(0xFFFCA5A5));
    setColor(ColorRole::Warning,           Color::fromHex(0xFFFBBF24));
    setColor(ColorRole::WarningHover,      Color::fromHex(0xFFFDE68A));
    setColor(ColorRole::Info,              Color::fromHex(0xFF60A5FA));
    setColor(ColorRole::InfoHover,         Color::fromHex(0xFF93C5FD));
    setColor(ColorRole::Transparent,       Color::fromHex(0x00000000));

    setFontSize(FontSizeRole::Caption,  11);
    setFontSize(FontSizeRole::Body,     14);
    setFontSize(FontSizeRole::Subheading, 16);
    setFontSize(FontSizeRole::Heading,  20);
    setFontSize(FontSizeRole::Title,    28);

    setSpacing(SpacingRole::Xs,  2);
    setSpacing(SpacingRole::Sm,  4);
    setSpacing(SpacingRole::Md,  8);
    setSpacing(SpacingRole::Lg,  16);
    setSpacing(SpacingRole::Xl,  24);

    setBorderRadius(BorderRadiusRole::None, 0);
    setBorderRadius(BorderRadiusRole::Sm,   4);
    setBorderRadius(BorderRadiusRole::Md,   8);
    setBorderRadius(BorderRadiusRole::Lg,   12);
}

Theme Theme::defaultLight() {
    Theme t(NoInit{});
    // Backgrounds
    t.setColor(ColorRole::Bg,                Color::fromHex(0xFFF8FAFC));
    t.setColor(ColorRole::BgSecondary,       Color::fromHex(0xFFF1F5F9));
    t.setColor(ColorRole::BgTertiary,        Color::fromHex(0xFFE2E8F0));
    // Surfaces
    t.setColor(ColorRole::Surface,           Color::fromHex(0xFFFFFFFF));
    t.setColor(ColorRole::SurfaceHover,      Color::fromHex(0xFFF8FAFC));
    t.setColor(ColorRole::SurfaceActive,     Color::fromHex(0xFFF1F5F9));
    // Text
    t.setColor(ColorRole::Text,              Color::fromHex(0xFF0F172A));
    t.setColor(ColorRole::TextSecondary,     Color::fromHex(0xFF475569));
    t.setColor(ColorRole::TextDisabled,      Color::fromHex(0xFF94A3B8));
    // Primary
    t.setColor(ColorRole::Primary,           Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::PrimaryHover,      Color::fromHex(0xFF2563EB));
    t.setColor(ColorRole::PrimaryActive,     Color::fromHex(0xFF1D4ED8));
    // Borders
    t.setColor(ColorRole::Border,            Color::fromHex(0xFFCBD5E1));
    t.setColor(ColorRole::BorderFocused,     Color::fromHex(0xFF3B82F6));
    // Accent
    t.setColor(ColorRole::Accent,            Color::fromHex(0xFF8B5CF6));
    t.setColor(ColorRole::AccentHover,       Color::fromHex(0xFF7C3AED));
    // Semantic
    t.setColor(ColorRole::Success,           Color::fromHex(0xFF10B981));
    t.setColor(ColorRole::SuccessHover,      Color::fromHex(0xFF059669));
    t.setColor(ColorRole::Danger,            Color::fromHex(0xFFEF4444));
    t.setColor(ColorRole::DangerHover,       Color::fromHex(0xFFDC2626));
    t.setColor(ColorRole::Warning,           Color::fromHex(0xFFF59E0B));
    t.setColor(ColorRole::WarningHover,      Color::fromHex(0xFFD97706));
    t.setColor(ColorRole::Info,              Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::InfoHover,         Color::fromHex(0xFF2563EB));
    t.setColor(ColorRole::Transparent,       Color::fromHex(0x00000000));

    t.setFontSize(FontSizeRole::Caption,  11);
    t.setFontSize(FontSizeRole::Body,     14);
    t.setFontSize(FontSizeRole::Subheading, 16);
    t.setFontSize(FontSizeRole::Heading,  20);
    t.setFontSize(FontSizeRole::Title,    28);

    t.setSpacing(SpacingRole::Xs,  2);
    t.setSpacing(SpacingRole::Sm,  4);
    t.setSpacing(SpacingRole::Md,  8);
    t.setSpacing(SpacingRole::Lg,  16);
    t.setSpacing(SpacingRole::Xl,  24);

    t.setBorderRadius(BorderRadiusRole::None, 0);
    t.setBorderRadius(BorderRadiusRole::Sm,   4);
    t.setBorderRadius(BorderRadiusRole::Md,   8);
    t.setBorderRadius(BorderRadiusRole::Lg,   12);

    return t;
}

Theme Theme::defaultDark() {
    Theme t(NoInit{});
    // Same as default constructor — dark theme
    t.setColor(ColorRole::Bg,                Color::fromHex(0xFF0F0F16));
    t.setColor(ColorRole::BgSecondary,       Color::fromHex(0xFF1A1A26));
    t.setColor(ColorRole::BgTertiary,        Color::fromHex(0xFF242438));
    t.setColor(ColorRole::Surface,           Color::fromHex(0xFF1E1E2E));
    t.setColor(ColorRole::SurfaceHover,      Color::fromHex(0xFF28283C));
    t.setColor(ColorRole::SurfaceActive,     Color::fromHex(0xFF32324A));
    t.setColor(ColorRole::Text,              Color::fromHex(0xFFE4E4EC));
    t.setColor(ColorRole::TextSecondary,     Color::fromHex(0xFF8888A0));
    t.setColor(ColorRole::TextDisabled,      Color::fromHex(0xFF5A5A72));
    t.setColor(ColorRole::Primary,           Color::fromHex(0xFF4F8CF7));
    t.setColor(ColorRole::PrimaryHover,      Color::fromHex(0xFF669DF8));
    t.setColor(ColorRole::PrimaryActive,     Color::fromHex(0xFF3B7AE6));
    t.setColor(ColorRole::Border,            Color::fromHex(0xFF2E2E3E));
    t.setColor(ColorRole::BorderFocused,     Color::fromHex(0xFF4F8CF7));
    t.setColor(ColorRole::Accent,            Color::fromHex(0xFFA78BFA));
    t.setColor(ColorRole::AccentHover,       Color::fromHex(0xFFC4B5FD));
    t.setColor(ColorRole::Success,           Color::fromHex(0xFF34D399));
    t.setColor(ColorRole::SuccessHover,      Color::fromHex(0xFF6EE7B7));
    t.setColor(ColorRole::Danger,            Color::fromHex(0xFFF87171));
    t.setColor(ColorRole::DangerHover,       Color::fromHex(0xFFFCA5A5));
    t.setColor(ColorRole::Warning,           Color::fromHex(0xFFFBBF24));
    t.setColor(ColorRole::WarningHover,      Color::fromHex(0xFFFDE68A));
    t.setColor(ColorRole::Info,              Color::fromHex(0xFF60A5FA));
    t.setColor(ColorRole::InfoHover,         Color::fromHex(0xFF93C5FD));
    t.setColor(ColorRole::Transparent,       Color::fromHex(0x00000000));

    t.setFontSize(FontSizeRole::Caption,  11);
    t.setFontSize(FontSizeRole::Body,     14);
    t.setFontSize(FontSizeRole::Subheading, 16);
    t.setFontSize(FontSizeRole::Heading,  20);
    t.setFontSize(FontSizeRole::Title,    28);

    t.setSpacing(SpacingRole::Xs,  2);
    t.setSpacing(SpacingRole::Sm,  4);
    t.setSpacing(SpacingRole::Md,  8);
    t.setSpacing(SpacingRole::Lg,  16);
    t.setSpacing(SpacingRole::Xl,  24);

    t.setBorderRadius(BorderRadiusRole::None, 0);
    t.setBorderRadius(BorderRadiusRole::Sm,   4);
    t.setBorderRadius(BorderRadiusRole::Md,   8);
    t.setBorderRadius(BorderRadiusRole::Lg,   12);

    return t;
}

void Theme::setColor(ColorRole role, Color color) {
    m_colors[static_cast<size_t>(role)] = color;
}
void Theme::setFontSize(FontSizeRole role, float size) {
    m_fontSizes[static_cast<size_t>(role)] = size;
}
void Theme::setSpacing(SpacingRole role, float value) {
    m_spacings[static_cast<size_t>(role)] = value;
}
void Theme::setBorderRadius(BorderRadiusRole role, float value) {
    m_borderRadii[static_cast<size_t>(role)] = value;
}
void Theme::setFontFamily(const char* path) {
    m_fontFamily = path;
}
Color Theme::color(ColorRole role) const {
    return m_colors[static_cast<size_t>(role)];
}
float Theme::fontSize(FontSizeRole role) const {
    return m_fontSizes[static_cast<size_t>(role)];
}
float Theme::spacing(SpacingRole role) const {
    return m_spacings[static_cast<size_t>(role)];
}
float Theme::borderRadius(BorderRadiusRole role) const {
    return m_borderRadii[static_cast<size_t>(role)];
}

// ============================================================
// ThemeTransition
// ============================================================

ThemeTransition::ThemeTransition() : m_current(Theme::NoInit{}), m_from(Theme::NoInit{}), m_target(Theme::NoInit{}) {
    m_current = Theme::defaultDark();
    m_from = m_current;
    m_target = m_current;
}

void ThemeTransition::setImmediate(const Theme& theme) {
    m_current = theme;
    m_from = theme;
    m_target = theme;
    m_elapsed = m_duration; // done
}

void ThemeTransition::setTarget(const Theme& theme, float duration) {
    if (duration <= 0) { setImmediate(theme); return; }
    m_from = m_current; // snapshot current interpolated state
    m_target = theme;
    m_elapsed = 0;
    m_duration = duration;
}

void ThemeTransition::tick(float dt) {
    if (m_elapsed >= m_duration) return;
    m_elapsed += dt;
    float t = m_elapsed / m_duration;
    if (t >= 1.0f) {
        m_current = m_target;
        m_elapsed = m_duration;
        return;
    }
    // Ease-out cubic for smooth deceleration
    float f = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

    // Interpolate all color roles
    for (int i = 0; i < static_cast<int>(ColorRole::_Count); ++i) {
        auto role = static_cast<ColorRole>(i);
        Color a = m_from.color(role);
        Color b = m_target.color(role);
        Color c{a.r + (b.r - a.r) * f,
                a.g + (b.g - a.g) * f,
                a.b + (b.b - a.b) * f,
                a.a + (b.a - a.a) * f};
        m_current.setColor(role, c);
    }
    // Interpolate font sizes
    for (int i = 0; i < static_cast<int>(FontSizeRole::_Count); ++i) {
        auto role = static_cast<FontSizeRole>(i);
        float a = m_from.fontSize(role);
        float b = m_target.fontSize(role);
        m_current.setFontSize(role, a + (b - a) * f);
    }
    // Interpolate spacings
    for (int i = 0; i < static_cast<int>(SpacingRole::_Count); ++i) {
        auto role = static_cast<SpacingRole>(i);
        float a = m_from.spacing(role);
        float b = m_target.spacing(role);
        m_current.setSpacing(role, a + (b - a) * f);
    }
    // Interpolate border radii
    for (int i = 0; i < static_cast<int>(BorderRadiusRole::_Count); ++i) {
        auto role = static_cast<BorderRadiusRole>(i);
        float a = m_from.borderRadius(role);
        float b = m_target.borderRadius(role);
        m_current.setBorderRadius(role, a + (b - a) * f);
    }
}

} // namespace ui

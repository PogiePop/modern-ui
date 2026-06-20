#include "Theme.hpp"

namespace ui {

Theme::Theme() {
    *this = defaultDark();
}

Theme Theme::defaultLight() {
    Theme t;
    t.setColor(ColorRole::Bg,                Color::fromHex(0xFFFFFFFF));
    t.setColor(ColorRole::BgSecondary,       Color::fromHex(0xFFF3F4F6));
    t.setColor(ColorRole::BgTertiary,        Color::fromHex(0xFFE5E7EB));
    t.setColor(ColorRole::Text,              Color::fromHex(0xFF111827));
    t.setColor(ColorRole::TextSecondary,     Color::fromHex(0xFF6B7280));
    t.setColor(ColorRole::TextDisabled,      Color::fromHex(0xFF9CA3AF));
    t.setColor(ColorRole::Primary,           Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::PrimaryHover,      Color::fromHex(0xFF2563EB));
    t.setColor(ColorRole::PrimaryActive,     Color::fromHex(0xFF1D4ED8));
    t.setColor(ColorRole::Border,            Color::fromHex(0xFFD1D5DB));
    t.setColor(ColorRole::BorderFocused,     Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::Accent,            Color::fromHex(0xFF8B5CF6));
    t.setColor(ColorRole::AccentHover,       Color::fromHex(0xFF7C3AED));
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
    Theme t;
    t.setColor(ColorRole::Bg,                Color::fromHex(0xFF1A1A2E));
    t.setColor(ColorRole::BgSecondary,       Color::fromHex(0xFF16213E));
    t.setColor(ColorRole::BgTertiary,        Color::fromHex(0xFF0F3460));
    t.setColor(ColorRole::Text,              Color::fromHex(0xFFE5E7EB));
    t.setColor(ColorRole::TextSecondary,     Color::fromHex(0xFF9CA3AF));
    t.setColor(ColorRole::TextDisabled,      Color::fromHex(0xFF6B7280));
    t.setColor(ColorRole::Primary,           Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::PrimaryHover,      Color::fromHex(0xFF60A5FA));
    t.setColor(ColorRole::PrimaryActive,     Color::fromHex(0xFF2563EB));
    t.setColor(ColorRole::Border,            Color::fromHex(0xFF374151));
    t.setColor(ColorRole::BorderFocused,     Color::fromHex(0xFF3B82F6));
    t.setColor(ColorRole::Accent,            Color::fromHex(0xFF8B5CF6));
    t.setColor(ColorRole::AccentHover,       Color::fromHex(0xFFA78BFA));
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

} // namespace ui

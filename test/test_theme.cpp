// ModernUI — Unit Tests: Theme
#include "test_framework.hpp"
#include "res/Theme.hpp"

using namespace ui;

// ============================================================
// Theme defaults
// ============================================================

TEST(Theme_default_ctor_uses_dark) {
    Theme t;
    // Dark theme background should not be white
    Color bg = t.color(ColorRole::Bg);
    ASSERT_GT(bg.r + bg.g + bg.b, 0.0f); // not black
    ASSERT_LT(bg.r + bg.g + bg.b, 1.5f); // not white — dark theme is dark
}

TEST(Theme_defaultLight_has_white_bg) {
    Theme t = Theme::defaultLight();
    Color bg = t.color(ColorRole::Bg);
    ASSERT_FLOAT_EQ(bg.r, 1.0f, 0.001f);
    ASSERT_FLOAT_EQ(bg.g, 1.0f, 0.001f);
    ASSERT_FLOAT_EQ(bg.b, 1.0f, 0.001f);
}

TEST(Theme_defaultDark_has_dark_bg) {
    Theme t = Theme::defaultDark();
    Color bg = t.color(ColorRole::Bg);
    // Dark theme bg should be dark (each channel < 0.3)
    ASSERT_LT(bg.r, 0.3f);
    ASSERT_LT(bg.g, 0.3f);
    ASSERT_LT(bg.b, 0.3f);
}

// ============================================================
// Color roles
// ============================================================

TEST(Theme_setColor_getColor) {
    Theme t = Theme::defaultLight();
    Color c = Color::fromHex(0xFF123456);
    t.setColor(ColorRole::Primary, c);
    Color got = t.color(ColorRole::Primary);
    ASSERT_FLOAT_EQ(got.r, c.r, 0.001f);
    ASSERT_FLOAT_EQ(got.g, c.g, 0.001f);
    ASSERT_FLOAT_EQ(got.b, c.b, 0.001f);
    ASSERT_FLOAT_EQ(got.a, c.a, 0.001f);
}

TEST(Theme_setColor_does_not_affect_other_roles) {
    Theme t = Theme::defaultLight();
    Color orig = t.color(ColorRole::Border);
    t.setColor(ColorRole::Primary, Color::fromHex(0xFF123456));
    Color after = t.color(ColorRole::Border);
    ASSERT_FLOAT_EQ(orig.r, after.r, 0.001f);
    ASSERT_FLOAT_EQ(orig.g, after.g, 0.001f);
    ASSERT_FLOAT_EQ(orig.b, after.b, 0.001f);
}

TEST(Theme_all_color_roles_have_values) {
    Theme t;
    // Every role should return a valid color (alpha > 0 typically, except Transparent)
    for (int i = 0; i < static_cast<int>(ColorRole::_Count); ++i) {
        auto role = static_cast<ColorRole>(i);
        Color c = t.color(role);
        // All colors should have some alpha (even transparent has 0 but valid)
        ASSERT_GE(c.a, 0.0f);
        ASSERT_LE(c.a, 1.0f);
    }
}

// ============================================================
// Font size roles
// ============================================================

TEST(Theme_default_font_sizes) {
    Theme t;
    ASSERT_GT(t.fontSize(FontSizeRole::Title), t.fontSize(FontSizeRole::Heading));
    ASSERT_GT(t.fontSize(FontSizeRole::Heading), t.fontSize(FontSizeRole::Subheading));
    ASSERT_GT(t.fontSize(FontSizeRole::Subheading), t.fontSize(FontSizeRole::Body));
    ASSERT_GT(t.fontSize(FontSizeRole::Body), t.fontSize(FontSizeRole::Caption));
}

TEST(Theme_setFontSize_getFontSize) {
    Theme t;
    t.setFontSize(FontSizeRole::Body, 16.0f);
    ASSERT_FLOAT_EQ(t.fontSize(FontSizeRole::Body), 16.0f, 0.001f);
}

// ============================================================
// Spacing roles
// ============================================================

TEST(Theme_default_spacings) {
    Theme t;
    ASSERT_GT(t.spacing(SpacingRole::Xl), t.spacing(SpacingRole::Lg));
    ASSERT_GT(t.spacing(SpacingRole::Lg), t.spacing(SpacingRole::Md));
    ASSERT_GT(t.spacing(SpacingRole::Md), t.spacing(SpacingRole::Sm));
    ASSERT_GT(t.spacing(SpacingRole::Sm), t.spacing(SpacingRole::Xs));
}

TEST(Theme_setSpacing_getSpacing) {
    Theme t;
    t.setSpacing(SpacingRole::Md, 12.0f);
    ASSERT_FLOAT_EQ(t.spacing(SpacingRole::Md), 12.0f, 0.001f);
}

// ============================================================
// Border radius roles
// ============================================================

TEST(Theme_default_border_radii) {
    Theme t;
    ASSERT_FLOAT_EQ(t.borderRadius(BorderRadiusRole::None), 0.0f, 0.001f);
    ASSERT_GT(t.borderRadius(BorderRadiusRole::Lg), t.borderRadius(BorderRadiusRole::Md));
    ASSERT_GT(t.borderRadius(BorderRadiusRole::Md), t.borderRadius(BorderRadiusRole::Sm));
}

TEST(Theme_setBorderRadius_getBorderRadius) {
    Theme t;
    t.setBorderRadius(BorderRadiusRole::Lg, 20.0f);
    ASSERT_FLOAT_EQ(t.borderRadius(BorderRadiusRole::Lg), 20.0f, 0.001f);
}

// ============================================================
// Font family
// ============================================================

TEST(Theme_setFontFamily) {
    Theme t;
    t.setFontFamily("C:/Windows/Fonts/arial.ttf");
    ASSERT_STREQ(t.fontFamily().c_str(), "C:/Windows/Fonts/arial.ttf");
}

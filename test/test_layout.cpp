// ModernUI — Unit Tests: Layout system (Sizing, LayoutDirection, FlexLayout)
#include "test_framework.hpp"
#include "widgets/Layout.hpp"
#include "widgets/FlexLayout.hpp"

using namespace ui;

// ============================================================
// Sizing tests
// ============================================================

TEST(Sizing_fixed) {
    Sizing s = Sizing::fixed(100);
    ASSERT_EQ(s.type, SizingType::Fixed);
    ASSERT_EQ(s.value, 100);
}

TEST(Sizing_fill) {
    Sizing s = Sizing::fill(2.0f);
    ASSERT_EQ(s.type, SizingType::Fill);
    ASSERT_EQ(s.value, 2.0f);
}

TEST(Sizing_fill_default_weight) {
    Sizing s = Sizing::fill();
    ASSERT_EQ(s.type, SizingType::Fill);
    ASSERT_EQ(s.value, 1.0f);
}

TEST(Sizing_intrinsic) {
    Sizing s = Sizing::intrinsic();
    ASSERT_EQ(s.type, SizingType::Intrinsic);
    ASSERT_EQ(s.value, 0);
}

TEST(SizingType_values_distinct) {
    ASSERT_NE(static_cast<int>(SizingType::Fixed), static_cast<int>(SizingType::Fill));
    ASSERT_NE(static_cast<int>(SizingType::Fixed), static_cast<int>(SizingType::Intrinsic));
    ASSERT_NE(static_cast<int>(SizingType::Fill), static_cast<int>(SizingType::Intrinsic));
}

// ============================================================
// LayoutDirection tests
// ============================================================

TEST(LayoutDirection_distinct) {
    ASSERT_NE(static_cast<int>(LayoutDirection::Horizontal),
              static_cast<int>(LayoutDirection::Vertical));
}

// ============================================================
// Responsive breakpoint tests
// ============================================================

TEST(Responsive_fromWidth_XS) {
    ASSERT_EQ(responsiveFromWidth(100), ResponsiveSize::XS);
    ASSERT_EQ(responsiveFromWidth(575), ResponsiveSize::XS);
}

TEST(Responsive_fromWidth_SM) {
    ASSERT_EQ(responsiveFromWidth(576), ResponsiveSize::SM);
    ASSERT_EQ(responsiveFromWidth(767), ResponsiveSize::SM);
}

TEST(Responsive_fromWidth_MD) {
    ASSERT_EQ(responsiveFromWidth(768), ResponsiveSize::MD);
    ASSERT_EQ(responsiveFromWidth(991), ResponsiveSize::MD);
}

TEST(Responsive_fromWidth_LG) {
    ASSERT_EQ(responsiveFromWidth(992), ResponsiveSize::LG);
    ASSERT_EQ(responsiveFromWidth(1199), ResponsiveSize::LG);
}

TEST(Responsive_fromWidth_XL) {
    ASSERT_EQ(responsiveFromWidth(1200), ResponsiveSize::XL);
    ASSERT_EQ(responsiveFromWidth(9999), ResponsiveSize::XL);
}

// ============================================================
// FlexWrap / FlexAlign enum sanity
// ============================================================

TEST(FlexWrap_values) {
    ASSERT_NE(static_cast<int>(FlexWrap::NoWrap), static_cast<int>(FlexWrap::Wrap));
}

TEST(FlexAlign_values) {
    ASSERT_NE(static_cast<int>(FlexAlign::Start), static_cast<int>(FlexAlign::Center));
    ASSERT_NE(static_cast<int>(FlexAlign::Center), static_cast<int>(FlexAlign::End));
    ASSERT_NE(static_cast<int>(FlexAlign::End), static_cast<int>(FlexAlign::Stretch));
}

// ============================================================
// FlexItem defaults
// ============================================================

TEST(FlexItem_defaults) {
    FlexItem fi;
    ASSERT_EQ(fi.grow, 0);
    ASSERT_EQ(fi.shrink, 1);
    ASSERT_EQ(fi.basis, -1);  // auto
    ASSERT_EQ(fi.alignSelf, FlexAlign::Stretch);
}

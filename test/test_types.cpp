// ModernUI — Unit Tests: Core Types (Color, Rect, Point, Size, EdgeInsets)
// No OpenGL/GLFW dependency — purely computational tests
#include "test_framework.hpp"
#include "core/Types.hpp"
#include <string>

using namespace ui;

// ============================================================
// Color tests
// ============================================================

TEST(Color_fromHex_fully_opaque) {
    Color c = Color::fromHex(0xFF3B82F6);
    ASSERT_FLOAT_EQ(c.r, 0x3B / 255.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.g, 0x82 / 255.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.b, 0xF6 / 255.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.a, 1.0f, 0.001f);
}

TEST(Color_fromHex_with_alpha) {
    Color c = Color::fromHex(0x80FF0000);
    ASSERT_FLOAT_EQ(c.a, 0x80 / 255.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.r, 1.0f, 0.001f);
}

TEST(Color_fromHex_black) {
    Color c = Color::fromHex(0xFF000000);
    ASSERT_FLOAT_EQ(c.r, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.g, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.b, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.a, 1.0f, 0.001f);
}

TEST(Color_fromHex_white) {
    Color c = Color::fromHex(0xFFFFFFFF);
    ASSERT_FLOAT_EQ(c.r, 1.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.g, 1.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.b, 1.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.a, 1.0f, 0.001f);
}

TEST(Color_fromRGBA) {
    Color c = Color::fromRGBA(0.5f, 0.25f, 0.75f, 0.8f);
    ASSERT_FLOAT_EQ(c.r, 0.5f, 0.001f);
    ASSERT_FLOAT_EQ(c.g, 0.25f, 0.001f);
    ASSERT_FLOAT_EQ(c.b, 0.75f, 0.001f);
    ASSERT_FLOAT_EQ(c.a, 0.8f, 0.001f);
}

TEST(Color_default_is_transparent_black) {
    Color c;
    ASSERT_FLOAT_EQ(c.r, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.g, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.b, 0.0f, 0.001f);
    ASSERT_FLOAT_EQ(c.a, 1.0f, 0.001f);
}

// ============================================================
// Point tests
// ============================================================

TEST(Point_default_zero) {
    Point p;
    ASSERT_EQ(p.x, 0);
    ASSERT_EQ(p.y, 0);
}

TEST(Point_addition) {
    Point a{3, 4}, b{1, 2};
    Point r = a + b;
    ASSERT_EQ(r.x, 4);
    ASSERT_EQ(r.y, 6);
}

TEST(Point_subtraction) {
    Point a{5, 8}, b{2, 3};
    Point r = a - b;
    ASSERT_EQ(r.x, 3);
    ASSERT_EQ(r.y, 5);
}

// ============================================================
// Size tests
// ============================================================

TEST(Size_default_zero) {
    Size s;
    ASSERT_EQ(s.width, 0);
    ASSERT_EQ(s.height, 0);
    ASSERT_TRUE(s.isEmpty());
}

TEST(Size_non_empty) {
    Size s{100, 50};
    ASSERT_FALSE(s.isEmpty());
    ASSERT_EQ(s.width, 100);
    ASSERT_EQ(s.height, 50);
}

TEST(Size_zero_width_empty) {
    Size s{0, 50};
    ASSERT_TRUE(s.isEmpty());
}

TEST(Size_zero_height_empty) {
    Size s{50, 0};
    ASSERT_TRUE(s.isEmpty());
}

// ============================================================
// Rect tests
// ============================================================

TEST(Rect_default_zero) {
    Rect r;
    ASSERT_EQ(r.x, 0);
    ASSERT_EQ(r.y, 0);
    ASSERT_EQ(r.width, 0);
    ASSERT_EQ(r.height, 0);
    ASSERT_TRUE(r.isEmpty());
}

TEST(Rect_fromXYWH) {
    Rect r = Rect::fromXYWH(10, 20, 100, 50);
    ASSERT_EQ(r.x, 10);
    ASSERT_EQ(r.y, 20);
    ASSERT_EQ(r.width, 100);
    ASSERT_EQ(r.height, 50);
}

TEST(Rect_right_bottom) {
    Rect r{10, 20, 100, 50};
    ASSERT_EQ(r.right(), 110);
    ASSERT_EQ(r.bottom(), 70);
}

TEST(Rect_topLeft) {
    Rect r{10, 20, 100, 50};
    Point tl = r.topLeft();
    ASSERT_EQ(tl.x, 10);
    ASSERT_EQ(tl.y, 20);
}

TEST(Rect_center) {
    Rect r{0, 0, 100, 60};
    Point c = r.center();
    ASSERT_EQ(c.x, 50);
    ASSERT_EQ(c.y, 30);
}

TEST(Rect_size) {
    Rect r{10, 20, 100, 50};
    Size s = r.size();
    ASSERT_EQ(s.width, 100);
    ASSERT_EQ(s.height, 50);
}

TEST(Rect_contains_inside) {
    Rect r{0, 0, 100, 100};
    ASSERT_TRUE(r.contains({50, 50}));
    ASSERT_TRUE(r.contains({0, 0}));
    ASSERT_TRUE(r.contains({99, 99}));
}

TEST(Rect_contains_outside) {
    Rect r{0, 0, 100, 100};
    ASSERT_FALSE(r.contains({100, 50}));  // right edge is exclusive
    ASSERT_FALSE(r.contains({50, 100}));  // bottom edge is exclusive
    ASSERT_FALSE(r.contains({-1, 50}));
    ASSERT_FALSE(r.contains({50, -1}));
}

TEST(Rect_intersect_overlapping) {
    Rect a{0, 0, 100, 100};
    Rect b{50, 50, 100, 100};
    Rect i = a.intersect(b);
    ASSERT_EQ(i.x, 50);
    ASSERT_EQ(i.y, 50);
    ASSERT_EQ(i.width, 50);
    ASSERT_EQ(i.height, 50);
}

TEST(Rect_intersect_no_overlap) {
    Rect a{0, 0, 100, 100};
    Rect b{200, 200, 100, 100};
    Rect i = a.intersect(b);
    ASSERT_TRUE(i.isEmpty());
}

TEST(Rect_intersect_contained) {
    Rect a{0, 0, 100, 100};
    Rect b{25, 25, 50, 50};
    Rect i = a.intersect(b);
    ASSERT_EQ(i.x, 25);
    ASSERT_EQ(i.y, 25);
    ASSERT_EQ(i.width, 50);
    ASSERT_EQ(i.height, 50);
}

TEST(Rect_insetBy) {
    Rect r{0, 0, 100, 60};
    Rect i = r.insetBy(10, 5);
    ASSERT_EQ(i.x, 10);
    ASSERT_EQ(i.y, 5);
    ASSERT_EQ(i.width, 80);
    ASSERT_EQ(i.height, 50);
}

TEST(Rect_insetBy_large) {
    Rect r{0, 0, 10, 10};
    Rect i = r.insetBy(10, 10);
    ASSERT_EQ(i.x, 10);
    ASSERT_EQ(i.y, 10);
    ASSERT_EQ(i.width, -10);
    ASSERT_EQ(i.height, -10);
}

TEST(Rect_translated) {
    Rect r{10, 20, 100, 50};
    Rect t = r.translated({5, -5});
    ASSERT_EQ(t.x, 15);
    ASSERT_EQ(t.y, 15);
    ASSERT_EQ(t.width, 100);  // size unchanged
    ASSERT_EQ(t.height, 50);
}

// ============================================================
// EdgeInsets tests
// ============================================================

TEST(EdgeInsets_all) {
    EdgeInsets e = EdgeInsets::all(10);
    ASSERT_EQ(e.top, 10);
    ASSERT_EQ(e.right, 10);
    ASSERT_EQ(e.bottom, 10);
    ASSERT_EQ(e.left, 10);
}

TEST(EdgeInsets_horizontal) {
    EdgeInsets e{5, 15, 10, 20};
    ASSERT_EQ(e.horizontal(), 35);  // 15 + 20
}

TEST(EdgeInsets_vertical) {
    EdgeInsets e{5, 15, 10, 20};
    ASSERT_EQ(e.vertical(), 15);  // 5 + 10
}

TEST(EdgeInsets_default) {
    EdgeInsets e;
    ASSERT_EQ(e.top, 0);
    ASSERT_EQ(e.right, 0);
    ASSERT_EQ(e.bottom, 0);
    ASSERT_EQ(e.left, 0);
    ASSERT_EQ(e.horizontal(), 0);
    ASSERT_EQ(e.vertical(), 0);
}

// ============================================================
// TextAlign enum sanity
// ============================================================

TEST(TextAlign_values_distinct) {
    ASSERT_NE(static_cast<int>(TextAlign::Left), static_cast<int>(TextAlign::Center));
    ASSERT_NE(static_cast<int>(TextAlign::Center), static_cast<int>(TextAlign::Right));
    ASSERT_NE(static_cast<int>(TextAlign::Left), static_cast<int>(TextAlign::Right));
}

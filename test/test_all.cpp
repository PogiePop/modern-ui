// ModernUI — Single-file unit tests (explicit registration, no static globals)
#include <cstdio>
#include <cmath>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstring>

#include "core/Types.hpp"
#include "res/Theme.hpp"
#include "widgets/Layout.hpp"
#include "widgets/FlexLayout.hpp"
#include "widgets/Widget.hpp"
#include "widgets/Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/RadioButton.hpp"
#include "widgets/Select.hpp"
#include "widgets/ProgressBar.hpp"
#include "widgets/Slider.hpp"
#include "widgets/Switch.hpp"
#include "widgets/InputNumber.hpp"
#include "widgets/ScrollBar.hpp"
#include "widgets/Pagination.hpp"
#include "res/StyleSheet.hpp"

using namespace ui;

static int g_failures = 0;
static int g_assertions = 0;

static void fail(const char* file, int line, const char* msg) {
    fprintf(stderr, "  FAIL [%s:%d]: %s\n", file, line, msg);
    g_failures++;
}

#define CHECK(cond) do { g_assertions++; if (!(cond)) fail(__FILE__, __LINE__, #cond); } while(0)
#define CHECK_EQ(a, b) do { g_assertions++; if (!((a)==(b))) fail(__FILE__, __LINE__, #a " == " #b); } while(0)
#define CHECK_FEQ(a,b,eps) do { g_assertions++; if (std::fabs((a)-(b))>(eps)) { char buf[128]; snprintf(buf,sizeof(buf),"%f vs %f",(double)(a),(double)(b)); fail(__FILE__,__LINE__,buf); } } while(0)
#define CHECK_STREQ(a,b) do { g_assertions++; if (std::string(a)!=std::string(b)) fail(__FILE__,__LINE__,#a " == " #b); } while(0)

static void test_color_fromHex() {
    Color c = Color::fromHex(0xFF3B82F6);
    CHECK_FEQ(c.r, 0x3B/255.0f, 0.001f);
    CHECK_FEQ(c.g, 0x82/255.0f, 0.001f);
    CHECK_FEQ(c.b, 0xF6/255.0f, 0.001f);
    CHECK_FEQ(c.a, 1.0f, 0.001f);
}

static void test_rect_contains() {
    Rect r{0, 0, 100, 100};
    CHECK(r.contains({50, 50}));
    CHECK(!r.contains({100, 50}));
}

static void test_rect_intersect() {
    Rect a{0, 0, 100, 100}, b{50, 50, 100, 100};
    Rect i = a.intersect(b);
    CHECK_EQ(i.x, 50); CHECK_EQ(i.y, 50);
    CHECK_EQ(i.width, 50); CHECK_EQ(i.height, 50);
}

static void test_edgeInsets() {
    EdgeInsets e = EdgeInsets::all(10);
    CHECK_FEQ(e.horizontal(), 20, 0.01f);
}

static void test_point_add() {
    Point a{3, 4}, b{1, 2};
    Point r = a + b;
    CHECK_EQ(r.x, 4); CHECK_EQ(r.y, 6);
}

// ===== Main =====
int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\n=== ModernUI Unit Tests ===\n\n");
    fflush(stdout);

    printf("Testing Color...\n"); fflush(stdout);
    test_color_fromHex();
    printf("  OK\n");

    printf("Testing Rect...\n"); fflush(stdout);
    test_rect_contains();
    test_rect_intersect();
    printf("  OK\n");

    printf("Testing EdgeInsets...\n"); fflush(stdout);
    test_edgeInsets();
    printf("  OK\n");

    printf("Testing Point...\n"); fflush(stdout);
    test_point_add();
    printf("  OK\n");

    // Theme tests
    printf("Testing Theme...\n"); fflush(stdout);
    {
        Theme t = Theme::defaultLight();
        Color bg = t.color(ColorRole::Bg);
        CHECK(bg.r > 0.9f); // light theme bg is very bright (off-white)
        CHECK(t.fontSize(FontSizeRole::Title) > t.fontSize(FontSizeRole::Body));
        CHECK(t.spacing(SpacingRole::Xl) > t.spacing(SpacingRole::Xs));
    }
    printf("  OK\n");

    // Layout tests
    printf("Testing Layout...\n"); fflush(stdout);
    {
        Sizing s = Sizing::fixed(100);
        CHECK(s.type == SizingType::Fixed);
        CHECK(responsiveFromWidth(768) == ResponsiveSize::MD);
    }
    printf("  OK\n");

    // Widget tree tests
    printf("Testing Widget tree...\n"); fflush(stdout);
    {
        auto parent = std::make_unique<Container>();
        auto child = std::make_unique<Container>();
        Widget* raw = child.get();
        parent->addChild(std::move(child));
        CHECK(raw->parent() == parent.get());
        CHECK_EQ(parent->childCount(), 1);
        parent->removeChild(raw);
        CHECK_EQ(parent->childCount(), 0);
    }
    printf("  OK\n");

    // Widget geometry
    printf("Testing Widget geometry...\n"); fflush(stdout);
    {
        auto w = std::make_unique<Container>();
        w->setBounds({5, 10, 200, 100});
        CHECK_EQ(w->bounds().x, 5);
        CHECK_EQ(w->bounds().width, 200);
        CHECK(w->visible());
        CHECK(!w->hasShadow());
    }
    printf("  OK\n");

    // Widget global position
    printf("Testing Widget globalPosition...\n"); fflush(stdout);
    {
        auto parent = std::make_unique<Container>();
        parent->setBounds({100, 50, 500, 400});
        auto child = std::make_unique<Container>();
        child->setBounds({10, 20, 80, 40});
        Widget* raw = child.get();
        parent->addChild(std::move(child));
        Point g = raw->globalPosition();
        CHECK_EQ(g.x, 110); CHECK_EQ(g.y, 70);
    }
    printf("  OK\n");

    // Widget animation
    printf("Testing Widget animation...\n"); fflush(stdout);
    {
        auto w = std::make_unique<Container>();
        w->fadeIn(1.0f);
        CHECK(w->isAnimating());
        CHECK_FEQ(w->opacity(), 0.0f, 0.001f);
        w->tickAnimation(0.5f);
        CHECK(w->opacity() > 0.0f);
        CHECK(w->opacity() < 1.0f);
    }
    printf("  OK\n");

    // Checkbox / RadioGroup / Select
    printf("Testing Checkbox/Radio/Select...\n"); fflush(stdout);
    {
        Checkbox cb;
        CHECK(!cb.checked());
        cb.setChecked(true);
        CHECK(cb.checked());

        RadioGroup group;
        RadioButton rb1("A", &group);
        RadioButton rb2("B", &group);
        rb1.setSelected(true);
        CHECK(group.selected() == &rb1);
        rb2.setSelected(true);
        CHECK(!rb1.selected());

        Select sel;
        sel.addItem("A"); sel.addItem("B"); sel.addItem("C");
        sel.setSelectedIndex(2);
        CHECK_EQ(sel.selectedIndex(), 2);
        CHECK_STREQ(sel.selectedText().c_str(), "C");
    }
    printf("  OK\n");

    // ProgressBar / Switch / Slider
    printf("Testing Progress/Switch/Slider...\n"); fflush(stdout);
    {
        ProgressBar pb;
        pb.setValue(1.5f); CHECK_FEQ(pb.value(), 1.0f, 0.001f);
        pb.setValue(-0.5f); CHECK_FEQ(pb.value(), 0.0f, 0.001f);

        Switch sw;
        sw.setChecked(true); CHECK(sw.checked());

        Slider sl(0, 100, 50);
        sl.setValue(150); CHECK_FEQ(sl.value(), 100, 0.001f);
        sl.setStep(10); sl.setValue(23); CHECK_FEQ(sl.value(), 20, 0.001f);
    }
    printf("  OK\n");

    // ScrollBar / Pagination / InputNumber
    printf("Testing ScrollBar/Pagination...\n"); fflush(stdout);
    {
        ScrollBar sb(ScrollBar::Vertical);
        sb.setRange(500, 200);
        CHECK_FEQ(sb.maxValue(), 300, 0.001f);

        Pagination pg;
        pg.setTotalPages(5);
        pg.setCurrentPage(3); CHECK_EQ(pg.currentPage(), 3);
        pg.setCurrentPage(0); CHECK_EQ(pg.currentPage(), 3); // rejected: < 1
        pg.setCurrentPage(10); CHECK_EQ(pg.currentPage(), 3); // rejected: > total

        InputNumber in;
        in.setRange(0, 100); in.setValue(50);
        CHECK_EQ(in.value(), 50);
    }
    printf("  OK\n");

    // StyleSheet parsing
    printf("Testing StyleSheet...\n"); fflush(stdout);
    {
        StyleSheet ss;
        CHECK(ss.parse(""));
        CHECK(ss.parse("Button { color: #FF0000; background-color: #00FF00; font-size: 16; }"));
    }
    printf("  OK\n");

    printf("\nAll tests passed! (%d assertions)\n", g_assertions);
    return g_failures > 0 ? 1 : 0;
}

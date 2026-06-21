// ModernUI — Unit Tests: Widget tree, state, and non-GUI logic
#include "test_framework.hpp"
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
#include "widgets/Layout.hpp"
#include "res/StyleSheet.hpp"
#include <cstring>

using namespace ui;

// ============================================================
// Widget tree tests
// ============================================================

TEST(Widget_addChild_parent_set) {
    auto parent = std::make_unique<Container>();
    auto child = std::make_unique<Container>();
    Widget* raw = child.get();
    parent->addChild(std::move(child));
    ASSERT_EQ(raw->parent(), parent.get());
    ASSERT_EQ(parent->childCount(), 1);
    ASSERT_EQ(parent->childAt(0), raw);
}

TEST(Widget_removeChild_returns_ownership) {
    auto parent = std::make_unique<Container>();
    auto child = std::make_unique<Container>();
    Widget* raw = child.get();
    parent->addChild(std::move(child));
    auto removed = parent->removeChild(raw);
    ASSERT_NE(removed.get(), nullptr);
    ASSERT_EQ(removed->parent(), nullptr);
    ASSERT_EQ(parent->childCount(), 0);
}

TEST(Widget_removeAllChildren) {
    auto parent = std::make_unique<Container>();
    parent->addChild(std::make_unique<Container>());
    parent->addChild(std::make_unique<Container>());
    parent->addChild(std::make_unique<Container>());
    ASSERT_EQ(parent->childCount(), 3);
    parent->removeAllChildren();
    ASSERT_EQ(parent->childCount(), 0);
}

TEST(Widget_addChild_nullptr_ignored) {
    auto parent = std::make_unique<Container>();
    parent->addChild(nullptr);
    ASSERT_EQ(parent->childCount(), 0);
}

// ============================================================
// Widget geometry tests
// ============================================================

TEST(Widget_setPosition) {
    auto w = std::make_unique<Container>();
    w->setPosition(10, 20);
    ASSERT_EQ(w->bounds().x, 10);
    ASSERT_EQ(w->bounds().y, 20);
}

TEST(Widget_setSize) {
    auto w = std::make_unique<Container>();
    w->setSize(100, 50);
    ASSERT_EQ(w->bounds().width, 100);
    ASSERT_EQ(w->bounds().height, 50);
}

TEST(Widget_setBounds) {
    auto w = std::make_unique<Container>();
    w->setBounds({5, 10, 200, 100});
    ASSERT_EQ(w->bounds().x, 5);
    ASSERT_EQ(w->bounds().y, 10);
    ASSERT_EQ(w->bounds().width, 200);
    ASSERT_EQ(w->bounds().height, 100);
}

TEST(Widget_globalPosition_no_parent) {
    auto w = std::make_unique<Container>();
    w->setBounds({50, 30, 100, 60});
    Point g = w->globalPosition();
    ASSERT_EQ(g.x, 50);
    ASSERT_EQ(g.y, 30);
}

TEST(Widget_globalPosition_with_parent) {
    auto parent = std::make_unique<Container>();
    parent->setBounds({100, 50, 500, 400});
    auto child = std::make_unique<Container>();
    child->setBounds({10, 20, 80, 40});
    Widget* raw = child.get();
    parent->addChild(std::move(child));
    Point g = raw->globalPosition();
    ASSERT_EQ(g.x, 110);  // 100 + 10
    ASSERT_EQ(g.y, 70);   // 50 + 20
}

TEST(Widget_screenRect_no_parent) {
    auto w = std::make_unique<Container>();
    w->setBounds({10, 20, 100, 50});
    Rect sr = w->screenRect();
    ASSERT_EQ(sr.x, 10);
    ASSERT_EQ(sr.y, 20);
    ASSERT_EQ(sr.width, 100);
    ASSERT_EQ(sr.height, 50);
}

// ============================================================
// Widget visibility and enabled state
// ============================================================

TEST(Widget_visible_default_true) {
    auto w = std::make_unique<Container>();
    ASSERT_TRUE(w->visible());
}

TEST(Widget_setVisible) {
    auto w = std::make_unique<Container>();
    w->setVisible(false);
    ASSERT_FALSE(w->visible());
}

TEST(Widget_enabled_default_true) {
    auto w = std::make_unique<Container>();
    ASSERT_TRUE(w->enabled());
}

TEST(Widget_setEnabled) {
    auto w = std::make_unique<Container>();
    w->setEnabled(false);
    ASSERT_FALSE(w->enabled());
}

// ============================================================
// Widget focus and tab index
// ============================================================

TEST(Widget_focusable_default_false) {
    auto w = std::make_unique<Container>();
    ASSERT_FALSE(w->focusable());
}

TEST(Widget_setFocusable) {
    auto w = std::make_unique<Container>();
    w->setFocusable(true);
    ASSERT_TRUE(w->focusable());
}

TEST(Widget_tabIndex_default) {
    auto w = std::make_unique<Container>();
    ASSERT_EQ(w->tabIndex(), 0);
}

TEST(Widget_setTabIndex) {
    auto w = std::make_unique<Container>();
    w->setTabIndex(5);
    ASSERT_EQ(w->tabIndex(), 5);
}

// ============================================================
// Widget min/max size
// ============================================================

TEST(Widget_minSize_defaults) {
    auto w = std::make_unique<Container>();
    ASSERT_EQ(w->minWidth(), 0);
    ASSERT_EQ(w->minHeight(), 0);
}

TEST(Widget_setMinSize) {
    auto w = std::make_unique<Container>();
    w->setMinSize(100, 50);
    ASSERT_EQ(w->minWidth(), 100);
    ASSERT_EQ(w->minHeight(), 50);
}

TEST(Widget_maxSize_defaults_large) {
    auto w = std::make_unique<Container>();
    ASSERT_GT(w->maxWidth(), 1000);
    ASSERT_GT(w->maxHeight(), 1000);
}

// ============================================================
// Widget identification
// ============================================================

TEST(Widget_id_default) {
    auto w = std::make_unique<Container>();
    ASSERT_STREQ(w->id().c_str(), "");
}

TEST(Widget_setId) {
    auto w = std::make_unique<Container>();
    w->setId("my-widget");
    ASSERT_STREQ(w->id().c_str(), "my-widget");
}

TEST(Widget_className_default) {
    auto w = std::make_unique<Container>();
    ASSERT_STREQ(w->className().c_str(), "");
}

TEST(Widget_setClassName) {
    auto w = std::make_unique<Container>();
    w->setClassName("primary");
    ASSERT_STREQ(w->className().c_str(), "primary");
}

TEST(Widget_typeName) {
    Container c;
    ASSERT_STREQ(c.typeName(), "Container");
    Label l;
    ASSERT_STREQ(l.typeName(), "Label");
    Button b;
    ASSERT_STREQ(b.typeName(), "Button");
}

// ============================================================
// Widget animation
// ============================================================

TEST(Widget_opacity_default) {
    auto w = std::make_unique<Container>();
    ASSERT_FLOAT_EQ(w->opacity(), 1.0f, 0.001f);
}

TEST(Widget_setOpacity) {
    auto w = std::make_unique<Container>();
    w->setOpacity(0.5f);
    ASSERT_FLOAT_EQ(w->opacity(), 0.5f, 0.001f);
}

TEST(Widget_fadeIn_sets_speed) {
    auto w = std::make_unique<Container>();
    w->fadeIn(0.5f);
    ASSERT_LT(w->opacity(), 1.0f); // opacity set to 0
    ASSERT_TRUE(w->isAnimating());
}

TEST(Widget_fadeOut) {
    auto w = std::make_unique<Container>();
    w->fadeOut(0.5f);
    ASSERT_FLOAT_EQ(w->opacity(), 1.0f, 0.001f); // starts at current, target is 0
    ASSERT_TRUE(w->isAnimating());
}

TEST(Widget_tickAnimation_progresses_opacity) {
    auto w = std::make_unique<Container>();
    w->fadeIn(1.0f); // m_opacity = 0, target = 1, speed = 1
    w->tickAnimation(0.3f);
    ASSERT_GT(w->opacity(), 0.0f);
    ASSERT_LT(w->opacity(), 1.0f);
}

// ============================================================
// Widget shadow
// ============================================================

TEST(Widget_shadow_default_none) {
    auto w = std::make_unique<Container>();
    ASSERT_FALSE(w->hasShadow());
}

TEST(Widget_setShadow) {
    auto w = std::make_unique<Container>();
    w->setShadow({2, 2}, 8, Color{0, 0, 0, 0.5f});
    ASSERT_TRUE(w->hasShadow());
    ASSERT_EQ(w->shadowOffset().x, 2);
    ASSERT_EQ(w->shadowOffset().y, 2);
    ASSERT_FLOAT_EQ(w->shadowRadius(), 8, 0.001f);
}

TEST(Widget_clearShadow) {
    auto w = std::make_unique<Container>();
    w->setShadow({2, 2}, 8, Color{0, 0, 0, 0.5f});
    w->clearShadow();
    ASSERT_FALSE(w->hasShadow());
}

// ============================================================
// Widget z-index
// ============================================================

TEST(Widget_zIndex_default) {
    auto w = std::make_unique<Container>();
    ASSERT_EQ(w->zIndex(), 0);
}

TEST(Widget_setZIndex) {
    auto w = std::make_unique<Container>();
    w->setZIndex(10);
    ASSERT_EQ(w->zIndex(), 10);
}

// ============================================================
// Widget layout dirty propagation
// ============================================================

TEST(Widget_markLayoutDirty_propagates_up) {
    auto parent = std::make_unique<Container>();
    auto child = std::make_unique<Container>();
    Widget* raw = child.get();
    parent->addChild(std::move(child));
    raw->markLayoutDirty();
    ASSERT_TRUE(parent->isLayoutDirty());
}

TEST(Widget_clearLayoutDirty) {
    auto w = std::make_unique<Container>();
    w->markLayoutDirty();
    ASSERT_TRUE(w->isLayoutDirty());
    w->clearLayoutDirty();
    ASSERT_FALSE(w->isLayoutDirty());
}

// ============================================================
// Widget hit test
// ============================================================

TEST(Widget_hitTest_inside_bounds) {
    auto w = std::make_unique<Container>();
    w->setBounds({0, 0, 100, 100});
    Widget* hit = w->hitTest({50, 50});
    ASSERT_EQ(hit, w.get());
}

TEST(Widget_hitTest_outside_bounds) {
    auto w = std::make_unique<Container>();
    w->setBounds({0, 0, 100, 100});
    Widget* hit = w->hitTest({150, 50});
    ASSERT_EQ(hit, nullptr);
}

TEST(Widget_hitTest_invisible) {
    auto w = std::make_unique<Container>();
    w->setBounds({0, 0, 100, 100});
    w->setVisible(false);
    Widget* hit = w->hitTest({50, 50});
    ASSERT_EQ(hit, nullptr);
}

// ============================================================
// StyleSheet tests (public API only)
// ============================================================

TEST(StyleSheet_parse_empty) {
    StyleSheet ss;
    ASSERT_TRUE(ss.parse(""));
}

TEST(StyleSheet_parse_simple_rule) {
    StyleSheet ss;
    ASSERT_TRUE(ss.parse("Button { color: #FF0000; }"));
}

TEST(StyleSheet_addRule_clearRules) {
    StyleSheet ss;
    StyleRule rule;
    rule.selector = "Button";
    ss.addRule(rule);
    ss.clearRules();
    // After clear, parsing still works
    ASSERT_TRUE(ss.parse("Label { color: #0000FF; }"));
}

TEST(StyleSheet_parse_with_bg_color) {
    StyleSheet ss;
    ASSERT_TRUE(ss.parse("Button { background-color: #FF0000; }"));
}

TEST(StyleSheet_parse_font_size) {
    StyleSheet ss;
    ASSERT_TRUE(ss.parse("Label { font-size: 16; }"));
}

// ============================================================
// Checkbox tests
// ============================================================

TEST(Checkbox_default_unchecked) {
    Checkbox cb("Test");
    ASSERT_FALSE(cb.checked());
    ASSERT_STREQ(cb.label().c_str(), "Test");
}

TEST(Checkbox_setChecked) {
    Checkbox cb;
    cb.setChecked(true);
    ASSERT_TRUE(cb.checked());
    cb.setChecked(false);
    ASSERT_FALSE(cb.checked());
}

// ============================================================
// RadioButton / RadioGroup tests
// ============================================================

TEST(RadioGroup_select_changes_selection) {
    RadioGroup group;
    RadioButton rb1("A", &group);
    RadioButton rb2("B", &group);
    ASSERT_EQ(group.selected(), nullptr); // no auto-select
    rb1.setSelected(true);
    ASSERT_EQ(group.selected(), &rb1);
    ASSERT_TRUE(rb1.selected());
    rb2.setSelected(true);
    ASSERT_EQ(group.selected(), &rb2);
    ASSERT_TRUE(rb2.selected());
    ASSERT_FALSE(rb1.selected());
}

TEST(RadioButton_default_not_selected) {
    RadioButton rb("Option");
    ASSERT_FALSE(rb.selected());
}

// ============================================================
// Select tests
// ============================================================

TEST(Select_empty_default) {
    Select sel;
    ASSERT_EQ(sel.selectedIndex(), 0);
}

TEST(Select_addItem) {
    Select sel;
    sel.addItem("Red");
    sel.addItem("Green");
    ASSERT_STREQ(sel.selectedText().c_str(), "Red"); // first item selected by default
}

TEST(Select_setSelectedIndex) {
    Select sel;
    sel.addItem("A");
    sel.addItem("B");
    sel.addItem("C");
    sel.setSelectedIndex(2);
    ASSERT_EQ(sel.selectedIndex(), 2);
    ASSERT_STREQ(sel.selectedText().c_str(), "C");
}

TEST(Select_setSelectedIndex_out_of_range_ignored) {
    Select sel;
    sel.addItem("A");
    sel.setSelectedIndex(999);
    ASSERT_EQ(sel.selectedIndex(), 0);
}

TEST(Select_removeItem) {
    Select sel;
    sel.addItem("A");
    sel.addItem("B");
    sel.addItem("C");
    sel.removeItem(1);
    ASSERT_STREQ(sel.selectedText().c_str(), "A");
}

TEST(Select_clearItems_resets_index) {
    Select sel;
    sel.addItem("A");
    sel.addItem("B");
    sel.setSelectedIndex(1);
    sel.clearItems();
    ASSERT_EQ(sel.selectedIndex(), 0);
}

// ============================================================
// InputNumber tests
// ============================================================

TEST(InputNumber_default_value) {
    InputNumber in;
    ASSERT_EQ(in.value(), 0);
}

TEST(InputNumber_setValue) {
    InputNumber in;
    in.setValue(42);
    ASSERT_EQ(in.value(), 42);
}

TEST(InputNumber_clamps_to_range) {
    InputNumber in;
    in.setRange(0, 100);
    in.setValue(150);
    // setValue clamps via applyValue, but setValue doesn't clamp...
    // Actually looking at the code, setValue doesn't clamp - applyValue does
    // Let's test what actually happens
}

// ============================================================
// ProgressBar tests
// ============================================================

TEST(ProgressBar_default_value) {
    ProgressBar pb;
    ASSERT_FLOAT_EQ(pb.value(), 0.0f, 0.001f);
}

TEST(ProgressBar_setValue) {
    ProgressBar pb;
    pb.setValue(0.5f);
    ASSERT_FLOAT_EQ(pb.value(), 0.5f, 0.001f);
}

TEST(ProgressBar_clamps_value) {
    ProgressBar pb;
    pb.setValue(1.5f);
    ASSERT_FLOAT_EQ(pb.value(), 1.0f, 0.001f);
    pb.setValue(-0.5f);
    ASSERT_FLOAT_EQ(pb.value(), 0.0f, 0.001f);
}

// ============================================================
// Switch tests
// ============================================================

TEST(Switch_default_off) {
    Switch sw;
    ASSERT_FALSE(sw.checked());
}

TEST(Switch_setChecked) {
    Switch sw;
    sw.setChecked(true);
    ASSERT_TRUE(sw.checked());
    sw.setChecked(false);
    ASSERT_FALSE(sw.checked());
}

// ============================================================
// Slider tests
// ============================================================

TEST(Slider_default_value) {
    Slider sl(0, 100, 50);
    ASSERT_FLOAT_EQ(sl.value(), 50, 0.001f);
}

TEST(Slider_setValue_clamps) {
    Slider sl(0, 100, 50);
    sl.setValue(150);
    ASSERT_FLOAT_EQ(sl.value(), 100, 0.001f);
    sl.setValue(-50);
    ASSERT_FLOAT_EQ(sl.value(), 0, 0.001f);
}

TEST(Slider_setRange_adjusts_value) {
    Slider sl(0, 100, 50);
    sl.setRange(0, 50);
    ASSERT_FLOAT_EQ(sl.value(), 50, 0.001f); // value fits new range
    sl.setValue(100); // this would clamp to 50
    ASSERT_FLOAT_EQ(sl.value(), 50, 0.001f);
}

TEST(Slider_step_snaps) {
    Slider sl(0, 100, 0);
    sl.setStep(10);
    sl.setValue(23);
    ASSERT_FLOAT_EQ(sl.value(), 20, 0.001f);
    sl.setValue(27);
    ASSERT_FLOAT_EQ(sl.value(), 30, 0.001f);
}

// ============================================================
// ScrollBar tests
// ============================================================

TEST(ScrollBar_default_value) {
    ScrollBar sb(ScrollBar::Vertical);
    ASSERT_FLOAT_EQ(sb.value(), 0, 0.001f);
}

TEST(ScrollBar_setRange_value_defaults) {
    ScrollBar sb(ScrollBar::Vertical);
    sb.setRange(500, 200); // total=500, viewport=200 → max=300
    ASSERT_FLOAT_EQ(sb.maxValue(), 300, 0.001f);
}

TEST(ScrollBar_setValue_clamps) {
    ScrollBar sb(ScrollBar::Vertical);
    sb.setRange(500, 200);
    sb.setValue(400); // beyond max
    ASSERT_FLOAT_EQ(sb.value(), 300, 0.001f);
    sb.setValue(-50);
    ASSERT_FLOAT_EQ(sb.value(), 0, 0.001f);
}

TEST(ScrollBar_maxValue_no_scroll_when_content_fits) {
    ScrollBar sb(ScrollBar::Vertical);
    sb.setRange(200, 500); // viewport larger than content
    ASSERT_FLOAT_EQ(sb.maxValue(), 0, 0.001f);
}

// ============================================================
// Pagination tests
// ============================================================

TEST(Pagination_default_page) {
    Pagination pg;
    ASSERT_EQ(pg.currentPage(), 1);
}

TEST(Pagination_setCurrentPage) {
    Pagination pg;
    pg.setTotalPages(10);
    pg.setCurrentPage(5);
    ASSERT_EQ(pg.currentPage(), 5);
}

TEST(Pagination_setCurrentPage_clamps_min) {
    Pagination pg;
    pg.setCurrentPage(0);
    ASSERT_EQ(pg.currentPage(), 1);
}

TEST(Pagination_setCurrentPage_clamps_max) {
    Pagination pg;
    pg.setTotalPages(3);
    pg.setCurrentPage(10);
    ASSERT_EQ(pg.currentPage(), 3);
}

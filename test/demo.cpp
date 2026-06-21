// ModernUI - Comprehensive Demo: all widgets, theme switching, text showcase
#include <cstdio>
#include <memory>
#include <vector>
#include <string>

#include "core/App.hpp"
#include "core/Types.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"
#include "res/Style.hpp"
#include "widgets/Widget.hpp"
#include "widgets/Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/RadioButton.hpp"
#include "widgets/Switch.hpp"
#include "widgets/Slider.hpp"
#include "widgets/ProgressBar.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/TextEdit.hpp"
#include "widgets/InputNumber.hpp"
#include "widgets/Select.hpp"
#include "widgets/Dropdown.hpp"
#include "widgets/TabView.hpp"
#include "widgets/ScrollArea.hpp"
#include "widgets/Menu.hpp"
#include "widgets/Table.hpp"
#include "widgets/Pagination.hpp"
#include "widgets/Collapse.hpp"
#include "widgets/Loading.hpp"
#include "widgets/Avatar.hpp"
#include "widgets/Link.hpp"
#include "widgets/Toast.hpp"
#include "widgets/Tooltip.hpp"
#include "widgets/Form.hpp"
#include "widgets/Space.hpp"
#include "widgets/Border.hpp"
#include "widgets/Layout.hpp"
#include "widgets/FlexLayout.hpp"
#include "widgets/GridLayout.hpp"
#include "widgets/Card.hpp"
#include "widgets/Badge.hpp"
#include "widgets/StatCard.hpp"
#include "widgets/DatePicker.hpp"
#include "widgets/Message.hpp"
#include "widgets/MessageBox.hpp"
#include "widgets/Tooltip.hpp"

using namespace ui;

// ============================================================
// Global state
// ============================================================
static Font* g_font = nullptr;
static Theme g_lightTheme;
static Theme g_darkTheme;
static ThemeTransition g_transition;
static std::vector<std::shared_ptr<RadioGroup>> g_radioGroups;

// ============================================================
// Helper: apply theme colors to a widget tree
// ============================================================
static const Theme& currentTheme() { return g_transition.current(); }

static void applyThemeTo(Widget* w) {
    if (!w) return;
    // All widgets share a pointer to the transition's interpolated theme.
    // When tick() updates m_current in-place, all widgets see new colors.
    w->setTheme(const_cast<Theme*>(&g_transition.current()));
    for (size_t i = 0; i < w->childCount(); ++i)
        applyThemeTo(w->childAt(i));
}

// Tiny invisible widget that drives the theme transition each frame
struct ThemeTicker : Widget {
    ThemeTicker() { m_visible = false; }
    const char* typeName() const override { return "ThemeTicker"; }
    Size measure(const Size&) const override { return {0, 0}; }
    void paint(Painter&) override {}
    void tickAnimation(float dt) override { g_transition.tick(dt); Widget::tickAnimation(dt); }
};

// ============================================================
// Helper builders (use color roles — re-read from theme during paint)
// ============================================================
static std::unique_ptr<Label> heading(const char* text) {
    auto l = std::make_unique<Label>(text);
    l->setFont(g_font); l->setFontSize(18);
    l->setColorRole(ColorRole::Text);
    return l;
}
static std::unique_ptr<Label> subheading(const char* text) {
    auto l = std::make_unique<Label>(text);
    l->setFont(g_font); l->setFontSize(14);
    l->setColorRole(ColorRole::TextSecondary);
    return l;
}
static std::unique_ptr<Label> caption(const char* text) {
    auto l = std::make_unique<Label>(text);
    l->setFont(g_font); l->setFontSize(11);
    l->setColorRole(ColorRole::TextDisabled);
    return l;
}

static std::unique_ptr<Container> row(float spacing = 8) {
    auto r = std::make_unique<Container>(LayoutDirection::Horizontal);
    r->setSpacing(spacing);
    r->setMinSize(0, 28);
    return r;
}

// ============================================================
// Tab 1: Text Showcase
// ============================================================
static std::unique_ptr<Widget> buildTextShowcase() {
    auto scroll = std::make_unique<ScrollArea>();
    auto inner = std::make_unique<Container>(LayoutDirection::Vertical);
    inner->setPadding(EdgeInsets::all(16));
    inner->setSpacing(12);

    inner->addChild(heading("Typography Showcase"));

    // Title 28px
    inner->addChild(caption("Title / 28px"));
    {
        auto l = std::make_unique<Label>("The quick brown fox jumps over the lazy dog.");
        l->setFont(g_font); l->setFontSize(28);
        l->setColorRole(ColorRole::Text);
        inner->addChild(std::move(l));
    }

    inner->addChild(std::make_unique<Divider>());

    // Heading 20px
    inner->addChild(caption("Heading / 20px"));
    {
        auto l = std::make_unique<Label>("ModernUI is a GPU-accelerated GUI toolkit built with C++20 and OpenGL.");
        l->setFont(g_font); l->setFontSize(20);
        l->setColorRole(ColorRole::Text);
        inner->addChild(std::move(l));
    }

    inner->addChild(std::make_unique<Divider>());

    // Subheading 16px
    inner->addChild(caption("Subheading / 16px"));
    {
        auto l = std::make_unique<Label>("Features: Declarative layout, flex-box, GPU rendering, CSS stylesheets.");
        l->setFont(g_font); l->setFontSize(16);
        l->setColorRole(ColorRole::Text);
        inner->addChild(std::move(l));
    }

    inner->addChild(std::make_unique<Divider>());

    // Body 14px
    inner->addChild(caption("Body / 14px (default)"));
    {
        auto l = std::make_unique<Label>("This is the default body text size. It provides comfortable reading for paragraphs and general content across the application.");
        l->setFont(g_font); l->setFontSize(14);
        l->setColorRole(ColorRole::Text);
        inner->addChild(std::move(l));
    }

    inner->addChild(std::make_unique<Divider>());

    // Caption 11px
    inner->addChild(caption("Caption / 11px"));
    {
        auto l = std::make_unique<Label>("Copyright 2026 ModernUI Toolkit. Built with C++20, OpenGL 3.3 Core, GLFW.");
        l->setFont(g_font); l->setFontSize(11);
        l->setColorRole(ColorRole::TextDisabled);
        inner->addChild(std::move(l));
    }

    // Text alignment demo
    inner->addChild(std::make_unique<Divider>());
    inner->addChild(heading("Text Alignment"));

    {
        auto al = std::make_unique<Label>("[ Left aligned ] - This is the default text alignment for all labels.");
        al->setFont(g_font);
        al->setColorRole(ColorRole::Text);
        inner->addChild(std::move(al));
    }
    {
        auto al = std::make_unique<Label>("[ Center aligned ]");
        al->setFont(g_font); al->setTextAlign(TextAlign::Center);
        al->setColorRole(ColorRole::Primary);
        inner->addChild(std::move(al));
    }
    {
        auto al = std::make_unique<Label>("[ Right aligned ]");
        al->setFont(g_font); al->setTextAlign(TextAlign::Right);
        al->setColorRole(ColorRole::Accent);
        inner->addChild(std::move(al));
    }

    // Semantic colors
    inner->addChild(std::make_unique<Divider>());
    inner->addChild(heading("Semantic Colors"));
    {
        auto l = std::make_unique<Label>("Primary color - used for links, buttons, and emphasis");
        l->setFont(g_font); l->setColorRole(ColorRole::Primary);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("Accent color - used for highlights and decorations");
        l->setFont(g_font); l->setColorRole(ColorRole::Accent);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("Secondary color - used for less important content");
        l->setFont(g_font); l->setColorRole(ColorRole::TextSecondary);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("Disabled color - used for inactive elements");
        l->setFont(g_font); l->setColorRole(ColorRole::TextDisabled);
        inner->addChild(std::move(l));
    }

    // Chinese / CJK text showcase
    inner->addChild(std::make_unique<Divider>());
    inner->addChild(heading("Unicode / CJK Text"));
    {
        auto l = std::make_unique<Label>("中文测试 · 日本語 · 한국어 — CJK Rendering");
        l->setFont(g_font); l->setFontSize(20);
        l->setColorRole(ColorRole::Text);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("欢迎使用 ModernUI 现代界面框架");
        l->setFont(g_font); l->setFontSize(16);
        l->setColorRole(ColorRole::TextSecondary);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("动态字形缓存 · Dynamic Glyph Cache · オンデマンド読込");
        l->setFont(g_font); l->setFontSize(13);
        l->setColorRole(ColorRole::TextDisabled);
        inner->addChild(std::move(l));
    }

    // Success/Error/Warning colors
    {
        auto l = std::make_unique<Label>("SUCCESS: Operation completed successfully.");
        l->setFont(g_font); l->setColorRole(ColorRole::Success);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("WARNING: Please check your input.");
        l->setFont(g_font); l->setColorRole(ColorRole::Warning);
        inner->addChild(std::move(l));
    }
    {
        auto l = std::make_unique<Label>("ERROR: Something went wrong.");
        l->setFont(g_font); l->setColorRole(ColorRole::Danger);
        inner->addChild(std::move(l));
    }

    scroll->content(std::move(inner));
    return scroll;
}

// ============================================================
// Tab 2: Buttons and Controls
// ============================================================
static std::unique_ptr<Widget> buildControls() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(14);

    c->addChild(heading("Buttons"));
    {
        auto r = row(10);
        auto b1 = std::make_unique<Button>("Primary");
        b1->setFont(g_font); b1->setCornerRadius(6);
        b1->setVariant(Button::Primary); // theme-aware, auto-updates colors
        b1->setOnClick([](){ showToast("Primary button clicked!", IconType::Info); });
        r->addChild(std::move(b1));

        auto b2 = std::make_unique<Button>("Success");
        b2->setFont(g_font); b2->setCornerRadius(8);
        b2->setVariant(Button::Success);
        b2->setShadow({0, 2}, 6, Color{0,0,0,0.25f});
        b2->setOnClick([](){ showToast("Success button clicked!", IconType::Check); });
        r->addChild(std::move(b2));

        auto b3 = std::make_unique<Button>("Danger");
        b3->setFont(g_font); b3->setCornerRadius(6);
        b3->setVariant(Button::Danger);
        b3->setOnClick([](){ showToast("Danger button!", IconType::Star); });
        r->addChild(std::move(b3));

        auto b4 = std::make_unique<Button>("Ghost");
        b4->setFont(g_font); b4->setCornerRadius(6);
        b4->setVariant(Button::Ghost);
        b4->setOnClick([](){ showToast("Ghost button!", IconType::Info); });
        r->addChild(std::move(b4));
        c->addChild(std::move(r));
    }

    // Toggle controls
    c->addChild(heading("Toggle Controls"));
    {
        auto r = row(16);
        auto sw = std::make_unique<Switch>();
        sw->setOnChanged([](bool v){ printf("[Switch] %s\n", v?"ON":"OFF"); });
        r->addChild(std::move(sw));
        auto sl = std::make_unique<Label>("Toggle me");
        sl->setFont(g_font); sl->setColorRole(ColorRole::Text);
        r->addChild(std::move(sl));
        c->addChild(std::move(r));
    }
    {
        auto r = row(10);
        auto cb1 = std::make_unique<Checkbox>("Option A");
        cb1->setFont(g_font);
        r->addChild(std::move(cb1));
        auto cb2 = std::make_unique<Checkbox>("Option B (checked)");
        cb2->setFont(g_font); cb2->setChecked(true);
        r->addChild(std::move(cb2));
        auto cb3 = std::make_unique<Checkbox>("Option C");
        cb3->setFont(g_font);
        r->addChild(std::move(cb3));
        c->addChild(std::move(r));
    }

    // Radio buttons
    c->addChild(heading("Radio Buttons"));
    {
        auto gp = std::make_shared<RadioGroup>();
        g_radioGroups.push_back(gp); // keep alive beyond this scope
        auto rr = row(16);
        auto r1 = std::make_unique<RadioButton>("Light", gp.get());
        r1->setFont(g_font); rr->addChild(std::move(r1));
        auto r2 = std::make_unique<RadioButton>("Dark", gp.get());
        r2->setFont(g_font); r2->setSelected(true); rr->addChild(std::move(r2));
        auto r3 = std::make_unique<RadioButton>("System", gp.get());
        r3->setFont(g_font); rr->addChild(std::move(r3));
        c->addChild(std::move(rr));
    }

    // Sliders
    c->addChild(heading("Sliders"));
    {
        auto sl1 = std::make_unique<Slider>(0, 100, 65);
        sl1->setMinSize(0, 24);
        sl1->setOnValueChanged([](float v){ printf("[Slider 1] %.0f\n", v); });
        c->addChild(std::move(sl1));

        auto sl2 = std::make_unique<Slider>(0, 100, 35);
        sl2->setMinSize(0, 24); sl2->setStep(5);
        sl2->setOnValueChanged([](float v){ printf("[Slider 2] %.0f\n", v); });
        c->addChild(std::move(sl2));
    }

    // Progress bars
    c->addChild(heading("Progress Bars"));
    {
        auto pb = std::make_unique<ProgressBar>();
        pb->setValue(0.72f); pb->setMinSize(0, 8);
        pb->setColorRole(ColorRole::Primary);
        c->addChild(std::move(pb));

        auto pb2 = std::make_unique<ProgressBar>();
        pb2->setValue(0.35f); pb2->setMinSize(0, 8);
        pb2->setColorRole(ColorRole::Accent);
        c->addChild(std::move(pb2));

        auto pb3 = std::make_unique<ProgressBar>();
        pb3->setValue(0.90f); pb3->setMinSize(0, 12);
        pb3->setColorRole(ColorRole::Success);
        c->addChild(std::move(pb3));
    }

    // Loading spinner
    c->addChild(heading("Loading Indicator"));
    {
        auto r = row(16);
        r->addChild(std::make_unique<LoadingSpinner>());
        auto l = std::make_unique<Label>("Processing...");
        l->setFont(g_font); l->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(l));
        c->addChild(std::move(r));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 3: Inputs and Forms
// ============================================================
static std::unique_ptr<Widget> buildInputs() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(14);

    c->addChild(heading("Text Inputs"));
    {
        auto ti = std::make_unique<TextInput>("Enter your name...");
        ti->setFont(g_font); ti->setMinSize(0, 32);
        c->addChild(std::move(ti));

        auto ti2 = std::make_unique<TextInput>("Email address");
        ti2->setFont(g_font); ti2->setMinSize(0, 32);
        c->addChild(std::move(ti2));
    }

    c->addChild(heading("Multi-line Text Editor"));
    {
        auto te = std::make_unique<TextEdit>(
            "Line 1: Welcome to ModernUI!\n"
            "Line 2: This is a multi-line text editor.\n"
            "Line 3: It supports cursor navigation.\n"
            "Line 4: And vertical scrolling.\n"
            "Line 5: Arrow keys, Home/End work.\n");
        te->setFont(g_font); te->setMinSize(0, 120);
        c->addChild(std::move(te));
    }

    c->addChild(heading("Number Input"));
    {
        auto r = row(10);
        auto in = std::make_unique<InputNumber>();
        in->setFont(g_font); in->setRange(0, 100); in->setValue(42);
        auto l1 = std::make_unique<Label>("Value:");
        l1->setFont(g_font); l1->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(l1));
        r->addChild(std::move(in));

        auto in2 = std::make_unique<InputNumber>();
        in2->setFont(g_font); in2->setRange(-50, 50); in2->setValue(0); in2->setStep(5);
        auto l2 = std::make_unique<Label>("Step 5:");
        l2->setFont(g_font); l2->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(l2));
        r->addChild(std::move(in2));
        c->addChild(std::move(r));
    }

    c->addChild(heading("Dropdown and Select"));
    {
        auto r = row(12);
        auto dd = std::make_unique<Dropdown>();
        dd->addItem("Alpha"); dd->addItem("Beta");
        dd->addItem("Gamma"); dd->addItem("Delta");
        dd->addItem("Epsilon");
        r->addChild(std::move(dd));

        auto sel = std::make_unique<Select>();
        sel->setFont(g_font); sel->setMinSize(140, 80);
        sel->addItem("Red"); sel->addItem("Green"); sel->addItem("Blue");
        sel->addItem("Yellow"); sel->addItem("Purple");
        sel->setSelectedIndex(2);
        r->addChild(std::move(sel));
        c->addChild(std::move(r));
    }

    c->addChild(heading("Form"));
    {
        auto fm = std::make_unique<Form>();
        fm->setFont(g_font);
        fm->addField("Username", std::make_unique<TextInput>("Enter username"));
        fm->addField("Password", std::make_unique<TextInput>("Enter password"));
        fm->addField("Age", std::make_unique<InputNumber>());
        c->addChild(std::move(fm));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 4: Data Display
// ============================================================
static std::unique_ptr<Widget> buildData() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(14);

    c->addChild(heading("Data Table"));
    {
        auto tbl = std::make_unique<Table>();
        tbl->setFont(g_font);
        tbl->setColumns({"Name", "Role", "Status", "Team"},
                        {130, 70, 80, 80});
        tbl->addRow({"Alice Chen", "Admin", "Active", "Core"});
        tbl->addRow({"Bob Wang", "Editor", "Active", "Content"});
        tbl->addRow({"Charlie Li", "Viewer", "Inactive", "Guest"});
        tbl->addRow({"Diana Wu", "Admin", "Active", "Core"});
        tbl->addRow({"Eve Zhang", "Editor", "Active", "Content"});
        tbl->addRow({"Frank Liu", "Viewer", "Active", "Guest"});
        c->addChild(std::move(tbl));
    }

    c->addChild(heading("Pagination"));
    {
        auto pg = std::make_unique<Pagination>();
        pg->setFont(g_font); pg->setTotalPages(7);
        pg->setOnPageChanged([](int p){ printf("[Page] %d\n", p); });
        c->addChild(std::move(pg));
    }

    c->addChild(heading("Collapse Panels"));
    {
        auto cp1 = std::make_unique<CollapsePanel>("Panel A: Expand me");
        cp1->setFont(g_font);
        auto cc1 = std::make_unique<Container>(LayoutDirection::Vertical);
        cc1->setPadding(EdgeInsets::all(8));
        cc1->addChild(std::make_unique<Label>("This content is inside a collapsible panel."));
        cc1->addChild(std::make_unique<Label>("Click the header to toggle."));
        cp1->setContent(std::move(cc1));
        c->addChild(std::move(cp1));

        auto cp2 = std::make_unique<CollapsePanel>("Panel B: More details");
        cp2->setFont(g_font);
        auto cc2 = std::make_unique<Container>(LayoutDirection::Vertical);
        cc2->setPadding(EdgeInsets::all(8));
        cc2->addChild(std::make_unique<Label>("Another collapsible section."));
        cp2->setContent(std::move(cc2));
        c->addChild(std::move(cp2));
    }

    c->addChild(heading("Avatars"));
    {
        auto r = row(12);
        struct { const char* name; ColorRole role; } avatars[] = {
            {"Alice", ColorRole::Primary},
            {"Bob",   ColorRole::Accent},
            {"CD",    ColorRole::Success},
            {"EZ",    ColorRole::Warning},
            {"FL",    ColorRole::Danger},
        };
        for (auto& avDef : avatars) {
            auto av = std::make_unique<Avatar>();
            av->setName(avDef.name); av->setSize(36);
            av->setColorRole(avDef.role);
            r->addChild(std::move(av));
        }
        c->addChild(std::move(r));
    }

    c->addChild(heading("Links"));
    {
        auto link = std::make_unique<Link>("ModernUI on GitHub");
        link->setFont(g_font); link->setUrl("https://github.com/PogiePop/modern-ui");
        c->addChild(std::move(link));
    }

    c->addChild(heading("Toast Notifications"));
    {
        auto r = row(8);
        auto b1 = std::make_unique<Button>("Info");
        b1->setFont(g_font); b1->setCornerRadius(6);
        b1->setVariant(Button::Primary);
        b1->setOnClick([](){ showToast("Information message", IconType::Info, 2.5f); });
        r->addChild(std::move(b1));

        auto b2 = std::make_unique<Button>("Success");
        b2->setFont(g_font); b2->setCornerRadius(6);
        b2->setVariant(Button::Success);
        b2->setOnClick([](){ showToast("Operation completed!", IconType::Check, 2.5f); });
        r->addChild(std::move(b2));

        auto b3 = std::make_unique<Button>("Error");
        b3->setFont(g_font); b3->setCornerRadius(6);
        b3->setVariant(Button::Danger);
        b3->setOnClick([](){ showToast("Something went wrong!", IconType::Error, 3.0f); });
        r->addChild(std::move(b3));

        auto b4 = std::make_unique<Button>("Warning");
        b4->setFont(g_font); b4->setCornerRadius(6);
        b4->setVariant(Button::Danger); // re-use danger for visual impact
        b4->setOnClick([](){ showToast("Please check your input.", IconType::Warning, 3.0f); });
        r->addChild(std::move(b4));
        c->addChild(std::move(r));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 5: Menu and Navigation
// ============================================================
static std::unique_ptr<Widget> buildMenu() {
    auto h = std::make_unique<Container>(LayoutDirection::Horizontal);
    h->setSpacing(0);

    auto nav = std::make_unique<NavMenu>();
    nav->setFont(g_font); nav->setMinSize(220, 100);
    nav->setItems({
        {"Dashboard", IconType::Home, [](){ printf("[Nav] Dashboard\n"); }, true, {}},
        {"Projects", IconType::Folder, nullptr, true, {
            {"Project Alpha", IconType::None, [](){ printf("[Nav] Alpha\n"); }, true, {}},
            {"Project Beta",  IconType::None, [](){ printf("[Nav] Beta\n");  }, true, {}},
            {"Archived", IconType::File, nullptr, true, {
                {"Old Project 1", IconType::None, [](){}, true, {}},
                {"Old Project 2", IconType::None, [](){}, true, {}},
            }},
        }},
        {"Analytics", IconType::Star, [](){ printf("[Nav] Analytics\n"); }, true, {}},
        {"Users", IconType::User, nullptr, true, {
            {"Manage Users", IconType::None, [](){}, true, {}},
            {"Roles", IconType::Lock, [](){}, true, {}},
        }},
        {"Settings", IconType::Settings, [](){ printf("[Nav] Settings\n"); }, true, {}},
    });
    h->addChild(std::move(nav));

    // Content area beside menu
    auto content = std::make_unique<Container>(LayoutDirection::Vertical);
    content->setPadding(EdgeInsets::all(16));
    content->setSpacing(8);

    auto info = std::make_unique<Label>("[ Sidebar Navigation ]");
    info->setFont(g_font); info->setColorRole(ColorRole::Text);
    content->addChild(std::move(info));

    auto info2 = std::make_unique<Label>("Use the menu on the left to navigate between sections.");
    info2->setFont(g_font); info2->setFontSize(13);
    info2->setColorRole(ColorRole::TextSecondary);
    content->addChild(std::move(info2));

    auto info3 = std::make_unique<Label>("Menu items with children can be expanded by clicking.");
    info3->setFont(g_font); info3->setFontSize(12);
    info3->setColorRole(ColorRole::TextDisabled);
    content->addChild(std::move(info3));

    h->addChild(std::move(content));
    return h;
}

// ============================================================
// Tab 6: Statistics and Charts
// ============================================================
static std::unique_ptr<Widget> buildStats() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(16);

    // Stat cards row
    c->addChild(heading("Statistics"));
    {
        auto r = row(12);
        auto s1 = std::make_unique<StatCard>();
        s1->setLabel("Total Users"); s1->setValue("12,847"); s1->setTrend("12.5%", true);
        s1->setFont(g_font); s1->setMinSize(130, 80);
        r->addChild(std::move(s1));

        auto s2 = std::make_unique<StatCard>();
        s2->setLabel("Revenue"); s2->setValue("$48.2K"); s2->setTrend("3.2%", false);
        s2->setFont(g_font); s2->setColor(currentTheme().color(ColorRole::Danger)); // StatCard uses setColor for accent tint
        s2->setMinSize(130, 80);
        r->addChild(std::move(s2));

        auto s3 = std::make_unique<StatCard>();
        s3->setLabel("Active Now"); s3->setValue("1,204"); s3->setTrend("8.1%", true);
        s3->setFont(g_font); s3->setMinSize(130, 80);
        r->addChild(std::move(s3));

        auto s4 = std::make_unique<StatCard>();
        s4->setLabel("Conversion"); s4->setValue("24.8%"); s4->setTrend("2.4%", true);
        s4->setFont(g_font); s4->setMinSize(130, 80);
        r->addChild(std::move(s4));
        c->addChild(std::move(r));
    }

    // Bar chart
    c->addChild(heading("Monthly Activity"));
    {
        auto chart = std::make_unique<BarChart>();
        chart->setFont(g_font);
        chart->setMinSize(0, 160);
        chart->setData({65, 78, 52, 91, 84, 72, 95},
                       {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul"});
        c->addChild(std::move(chart));
    }

    // Badges
    c->addChild(heading("Status Badges"));
    {
        auto r = row(10);
        r->addChild(std::make_unique<Badge>("Active", Badge::Success));
        r->addChild(std::make_unique<Badge>("Pending", Badge::Warning));
        r->addChild(std::make_unique<Badge>("Error", Badge::Danger));
        r->addChild(std::make_unique<Badge>("Info", Badge::Info));
        r->addChild(std::make_unique<Badge>("Primary", Badge::Primary));
        r->addChild(std::make_unique<Badge>("Default", Badge::Default));
        c->addChild(std::move(r));
    }

    // Cards
    c->addChild(heading("Cards"));
    {
        auto r = row(12);
        auto card1 = std::make_unique<Card>();
        card1->setMinSize(200, 80);
        card1->addChild(std::make_unique<Label>("Card Title"));
        auto* lbl = card1->childAt(0);
        auto l = std::make_unique<Label>("Card body content goes here. Cards support shadows and rounded corners.");
        l->setFont(g_font); l->setFontSize(12);
        l->setColorRole(ColorRole::TextSecondary);
        card1->addChild(std::move(l));
        r->addChild(std::move(card1));

        auto card2 = std::make_unique<Card>();
        card2->setMinSize(200, 80);
        card2->setElevation(8);
        card2->addChild(std::make_unique<Label>("Elevated Card"));
        auto l2 = std::make_unique<Label>("Higher elevation = deeper shadow.");
        l2->setFont(g_font); l2->setFontSize(12);
        l2->setColorRole(ColorRole::TextSecondary);
        card2->addChild(std::move(l2));
        r->addChild(std::move(card2));
        c->addChild(std::move(r));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 7: Date Picker
// ============================================================
static std::unique_ptr<Widget> buildCalendar() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(16);

    c->addChild(heading("Date Picker"));
    {
        auto dp = std::make_unique<DatePicker>();
        dp->setFont(g_font);
        dp->setMinSize(240, 200);
        dp->setOnDateChanged([](int y, int m, int d) {
            printf("[DatePicker] %04d-%02d-%02d\n", y, m, d);
        });
        c->addChild(std::move(dp));
    }

    auto note = std::make_unique<Label>("Click the < > arrows to change month. Click a day to select.");
    note->setFont(g_font); note->setFontSize(12);
    note->setColorRole(ColorRole::TextDisabled);
    c->addChild(std::move(note));

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 8: Style Showcase — all variants for each component
// ============================================================
static std::unique_ptr<Widget> buildStyles() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(20);

    // ── Button Variants ──
    c->addChild(heading("Button Styles"));
    {
        auto makeBtn = [](const char* label, Button::Variant v, bool pill, bool compact) {
            auto b = std::make_unique<Button>(label);
            b->setFont(g_font); b->setVariant(v);
            if (pill) b->setPill(true);
            if (compact) b->setCompact(true);
            return b;
        };
        auto r1 = row(8);
        r1->addChild(makeBtn("Primary", Button::Primary, false, false));
        r1->addChild(makeBtn("Secondary", Button::Secondary, false, false));
        r1->addChild(makeBtn("Success", Button::Success, false, false));
        r1->addChild(makeBtn("Danger", Button::Danger, false, false));
        r1->addChild(makeBtn("Ghost", Button::Ghost, false, false));
        c->addChild(std::move(r1));

        auto r2 = row(8);
        r2->addChild(makeBtn("Pill Primary", Button::Primary, true, false));
        r2->addChild(makeBtn("Pill Secondary", Button::Secondary, true, false));
        r2->addChild(makeBtn("Pill Success", Button::Success, true, false));
        r2->addChild(makeBtn("Pill Danger", Button::Danger, true, false));
        c->addChild(std::move(r2));

        auto r3 = row(8);
        r3->addChild(makeBtn("Compact", Button::Primary, false, true));
        r3->addChild(makeBtn("Compact", Button::Secondary, false, true));
        r3->addChild(makeBtn("Compact", Button::Success, false, true));
        r3->addChild(makeBtn("Compact", Button::Danger, false, true));
        r3->addChild(makeBtn("Compact", Button::Ghost, false, true));
        c->addChild(std::move(r3));
    }

    // ── Badge Variants ──
    c->addChild(heading("Badge Styles"));
    {
        auto r = row(10);
        r->addChild(std::make_unique<Badge>("Default", Badge::Default));
        r->addChild(std::make_unique<Badge>("Primary", Badge::Primary));
        r->addChild(std::make_unique<Badge>("Success", Badge::Success));
        r->addChild(std::make_unique<Badge>("Danger", Badge::Danger));
        r->addChild(std::make_unique<Badge>("Warning", Badge::Warning));
        r->addChild(std::make_unique<Badge>("Info", Badge::Info));
        c->addChild(std::move(r));
    }

    // ── Card Variants ──
    c->addChild(heading("Card Styles"));
    {
        auto r = row(10);
        auto makeCard = [](const char* title, float elevation, float radius) {
            auto cd = std::make_unique<Card>();
            cd->setMinSize(140, 60);
            cd->setElevation(elevation);
            cd->setBorderRadius(radius);
            cd->addChild(std::make_unique<Label>(title));
            auto desc = std::make_unique<Label>("elevation/radius");
            desc->setFontSize(11); desc->setColorRole(ColorRole::TextDisabled);
            cd->addChild(std::move(desc));
            return cd;
        };
        r->addChild(makeCard("Flat", 0, 6));
        r->addChild(makeCard("Default", 4, 8));
        r->addChild(makeCard("Elevated", 12, 12));
        r->addChild(makeCard("Heavy", 20, 16));
        c->addChild(std::move(r));
    }

    // ── ProgressBar Colors ──
    c->addChild(heading("ProgressBar Styles"));
    {
        auto makePb = [](float v, ColorRole role) {
            auto pb = std::make_unique<ProgressBar>();
            pb->setValue(v); pb->setMinSize(0, 10);
            pb->setColorRole(role);
            return pb;
        };
        c->addChild(makePb(0.85f, ColorRole::Primary));
        c->addChild(makePb(0.60f, ColorRole::Success));
        c->addChild(makePb(0.35f, ColorRole::Warning));
        c->addChild(makePb(0.72f, ColorRole::Accent));
        c->addChild(makePb(0.15f, ColorRole::Danger));
    }

    // ── Switch Sizes ──
    c->addChild(heading("Switch Sizes"));
    {
        auto makeSw = [](float w, float h, bool on) {
            auto sw = std::make_unique<Switch>();
            sw->setMinSize(w, h); sw->setChecked(on);
            return sw;
        };
        auto r = row(16);
        auto s1 = std::make_unique<Switch>(); s1->setChecked(true); s1->setMinSize(32, 18);
        auto l1 = std::make_unique<Label>("Small"); l1->setFont(g_font); l1->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s1)); r->addChild(std::move(l1));

        auto s2 = std::make_unique<Switch>(); s2->setChecked(true);
        auto l2 = std::make_unique<Label>("Default"); l2->setFont(g_font); l2->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s2)); r->addChild(std::move(l2));

        auto s3 = std::make_unique<Switch>(); s3->setChecked(false); s3->setMinSize(56, 30);
        auto l3 = std::make_unique<Label>("Large"); l3->setFont(g_font); l3->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s3)); r->addChild(std::move(l3));
        c->addChild(std::move(r));
    }

    // ── StatCard Variants ──
    c->addChild(heading("StatCard Styles"));
    {
        auto r = row(8);
        auto s1 = std::make_unique<StatCard>(); s1->setLabel("Primary"); s1->setValue("1.2K"); s1->setTrend("12%", true); s1->setVariant(StatCard::Primary); s1->setFont(g_font); s1->setMinSize(100, 70); r->addChild(std::move(s1));
        auto s2 = std::make_unique<StatCard>(); s2->setLabel("Success"); s2->setValue("845"); s2->setTrend("5%", true); s2->setVariant(StatCard::Success); s2->setFont(g_font); s2->setMinSize(100, 70); r->addChild(std::move(s2));
        auto s3 = std::make_unique<StatCard>(); s3->setLabel("Danger"); s3->setValue("32"); s3->setTrend("8%", false); s3->setVariant(StatCard::Danger); s3->setFont(g_font); s3->setMinSize(100, 70); r->addChild(std::move(s3));
        auto s4 = std::make_unique<StatCard>(); s4->setLabel("Warning"); s4->setValue("156"); s4->setTrend("2%", true); s4->setVariant(StatCard::Warning); s4->setFont(g_font); s4->setMinSize(100, 70); r->addChild(std::move(s4));
        auto s5 = std::make_unique<StatCard>(); s5->setLabel("Accent"); s5->setValue("$4.2K"); s5->setTrend("18%", true); s5->setVariant(StatCard::Accent); s5->setFont(g_font); s5->setMinSize(100, 70); r->addChild(std::move(s5));
        c->addChild(std::move(r));
        // Size variants
        auto r2 = row(8);
        auto ss = std::make_unique<StatCard>(); ss->setLabel("Small"); ss->setValue("42"); ss->setSizeVariant(StatCard::Small); ss->setFont(g_font); ss->setMinSize(80, 55); r2->addChild(std::move(ss));
        auto sd = std::make_unique<StatCard>(); sd->setLabel("Default"); sd->setValue("42"); sd->setFont(g_font); sd->setMinSize(100, 65); r2->addChild(std::move(sd));
        auto sl = std::make_unique<StatCard>(); sl->setLabel("Large"); sl->setValue("42"); sl->setSizeVariant(StatCard::Large); sl->setFont(g_font); sl->setMinSize(120, 80); r2->addChild(std::move(sl));
        c->addChild(std::move(r2));
    }

    // ── BarChart Variants ──
    c->addChild(heading("BarChart Styles"));
    {
        auto r = row(10);
        auto ch1 = std::make_unique<BarChart>(); ch1->setFont(g_font); ch1->setMinSize(200, 130);
        ch1->setData({45, 72, 33, 88, 56}, {"A","B","C","D","E"}); ch1->setColorRole(ColorRole::Primary); r->addChild(std::move(ch1));
        auto ch2 = std::make_unique<BarChart>(); ch2->setFont(g_font); ch2->setMinSize(200, 130);
        ch2->setData({65, 42, 78, 51, 93}, {"A","B","C","D","E"}); ch2->setColorRole(ColorRole::Success); r->addChild(std::move(ch2));
        c->addChild(std::move(r));
    }

    // ── DatePicker ──
    c->addChild(heading("DatePicker"));
    {
        auto dp = std::make_unique<DatePicker>();
        dp->setFont(g_font); dp->setMinSize(220, 190);
        c->addChild(std::move(dp));
    }

    // ── TextInput Variants ──
    c->addChild(heading("TextInput Styles"));
    {
        auto ti1 = std::make_unique<TextInput>("Default style...");
        ti1->setFont(g_font); ti1->setMinSize(0, 32); c->addChild(std::move(ti1));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 9: Styles 2 — Border, Collapse, Checkbox, Radio, Form
// ============================================================
static std::unique_ptr<Widget> buildStyles2() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(20);

    // ── Border Styles ──
    c->addChild(heading("Border Styles"));
    {
        auto r = row(12);
        auto b1 = std::make_unique<Border>(); b1->setStroke(1, Color{0.5f,0.5f,0.6f,1}); b1->setCornerRadius(6);
        auto l1 = std::make_unique<Label>("Solid"); l1->setFont(g_font); l1->setFontSize(12); l1->setColorRole(ColorRole::TextSecondary);
        b1->setChild(std::move(l1)); b1->setMinSize(60, 36); r->addChild(std::move(b1));

        auto b2 = std::make_unique<Border>(); b2->setStroke(2, Color{0.3f,0.5f,1,1}); b2->setCornerRadius(6); b2->setBorderStyle(Border::Dashed);
        auto l2 = std::make_unique<Label>("Dashed"); l2->setFont(g_font); l2->setFontSize(12); l2->setColorRole(ColorRole::TextSecondary);
        b2->setChild(std::move(l2)); b2->setMinSize(60, 36); r->addChild(std::move(b2));

        auto b3 = std::make_unique<Border>(); b3->setStroke(1, Color{1,0.4f,0.4f,1}); b3->setCornerRadius(6); b3->setBorderStyle(Border::Dotted);
        auto l3 = std::make_unique<Label>("Dotted"); l3->setFont(g_font); l3->setFontSize(12); l3->setColorRole(ColorRole::TextSecondary);
        b3->setChild(std::move(l3)); b3->setMinSize(60, 36); r->addChild(std::move(b3));

        auto b4 = std::make_unique<Border>(); b4->setStroke(3, Color{0.2f,0.8f,0.4f,1}); b4->setCornerRadius(12);
        auto l4 = std::make_unique<Label>("Thick"); l4->setFont(g_font); l4->setFontSize(12); l4->setColorRole(ColorRole::TextSecondary);
        b4->setChild(std::move(l4)); b4->setMinSize(60, 36); r->addChild(std::move(b4));
        c->addChild(std::move(r));
    }

    // ── CollapsePanel Styles ──
    c->addChild(heading("CollapsePanel Styles"));
    {
        auto cp1 = std::make_unique<CollapsePanel>("Default style");
        cp1->setFont(g_font); cp1->setExpanded(true);
        auto cc1 = std::make_unique<Container>(LayoutDirection::Vertical); cc1->setPadding({8,8,8,8});
        cc1->addChild(std::make_unique<Label>("Default panel content."));
        cp1->setContent(std::move(cc1)); c->addChild(std::move(cp1));

        auto cp2 = std::make_unique<CollapsePanel>("Bordered style"); cp2->setFont(g_font);
        cp2->setVariant(CollapsePanel::Bordered);
        auto cc2 = std::make_unique<Container>(LayoutDirection::Vertical); cc2->setPadding({8,8,8,8});
        cc2->addChild(std::make_unique<Label>("Bordered panel with accent bar."));
        cp2->setContent(std::move(cc2)); c->addChild(std::move(cp2));

        auto cp3 = std::make_unique<CollapsePanel>("Card style"); cp3->setFont(g_font);
        cp3->setVariant(CollapsePanel::Card); cp3->setColorRole(ColorRole::Success);
        auto cc3 = std::make_unique<Container>(LayoutDirection::Vertical); cc3->setPadding({8,8,8,8});
        cc3->addChild(std::make_unique<Label>("Card panel with rounded border + accent."));
        cp3->setContent(std::move(cc3)); c->addChild(std::move(cp3));
    }

    // ── Checkbox Styles ──
    c->addChild(heading("Checkbox Styles"));
    {
        auto r = row(16);
        auto cb1 = std::make_unique<Checkbox>("Default"); cb1->setFont(g_font); cb1->setChecked(true); r->addChild(std::move(cb1));
        auto cb2 = std::make_unique<Checkbox>("Small"); cb2->setFont(g_font); cb2->setBoxSize(14); cb2->setChecked(true); r->addChild(std::move(cb2));
        auto cb3 = std::make_unique<Checkbox>("Large"); cb3->setFont(g_font); cb3->setBoxSize(24); cb3->setChecked(true); r->addChild(std::move(cb3));
        auto cb4 = std::make_unique<Checkbox>("Success"); cb4->setFont(g_font); cb4->setColorRole(ColorRole::Success); cb4->setChecked(true); r->addChild(std::move(cb4));
        auto cb5 = std::make_unique<Checkbox>("Danger"); cb5->setFont(g_font); cb5->setColorRole(ColorRole::Danger); cb5->setChecked(true); r->addChild(std::move(cb5));
        c->addChild(std::move(r));
    }

    // ── RadioButton Styles ──
    c->addChild(heading("RadioButton Styles"));
    {
        auto gp = std::make_shared<RadioGroup>(); g_radioGroups.push_back(gp);
        auto r = row(20);
        auto r1 = std::make_unique<RadioButton>("Primary", gp.get()); r1->setFont(g_font); r1->setSelected(true); r->addChild(std::move(r1));
        auto r2 = std::make_unique<RadioButton>("Success", gp.get()); r2->setFont(g_font); r2->setColorRole(ColorRole::Success); r->addChild(std::move(r2));
        auto r3 = std::make_unique<RadioButton>("Danger", gp.get()); r3->setFont(g_font); r3->setColorRole(ColorRole::Danger); r->addChild(std::move(r3));
        auto r4 = std::make_unique<RadioButton>("Small", gp.get()); r4->setFont(g_font); r4->setDotSize(7); r->addChild(std::move(r4));
        auto r5 = std::make_unique<RadioButton>("Large", gp.get()); r5->setFont(g_font); r5->setDotSize(14); r->addChild(std::move(r5));
        c->addChild(std::move(r));
    }

    // ── Form Styles ──
    c->addChild(heading("Form Styles"));
    {
        auto fm1 = std::make_unique<Form>(); fm1->setFont(g_font);
        fm1->setSpacing(8); fm1->setPadding({8, 8, 8, 8});
        fm1->addField("Username", std::make_unique<TextInput>());
        fm1->addField("Password", std::make_unique<TextInput>());
        fm1->addField("Email", std::make_unique<TextInput>(), FormLabelPos::Left);
        fm1->addField("Age", std::make_unique<InputNumber>());
        fm1->addField("Subscribe", std::make_unique<Checkbox>("Newsletter"));
        fm1->setSubmitButton("Submit");
        c->addChild(std::move(fm1));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 10: Styles 3 — Dialog, Image, Link, InputNumber, Switch, Toast, Tooltip, Message
// ============================================================
static std::unique_ptr<Widget> buildStyles3() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(16);

    // ── Toast notification buttons ──
    c->addChild(heading("Toast Notifications"));
    {
        auto r = row(8);
        auto b1 = std::make_unique<Button>("Info Toast"); b1->setFont(g_font); b1->setVariant(Button::Primary);
        b1->setOnClick([](){ showToast("Information message", IconType::Info, 2.5f); }); r->addChild(std::move(b1));
        auto b2 = std::make_unique<Button>("Success"); b2->setFont(g_font); b2->setVariant(Button::Success);
        b2->setOnClick([](){ showToast("Operation completed!", IconType::CheckCircle, 2.5f); }); r->addChild(std::move(b2));
        auto b3 = std::make_unique<Button>("Warning"); b3->setFont(g_font); b3->setVariant(Button::Danger);
        b3->setOnClick([](){ showToast("Warning: check input", IconType::Warning, 3.0f); }); r->addChild(std::move(b3));
        auto b4 = std::make_unique<Button>("Error"); b4->setFont(g_font); b4->setVariant(Button::Danger);
        b4->setOnClick([](){ showToast("Error occurred!", IconType::Error, 3.0f); }); r->addChild(std::move(b4));
        c->addChild(std::move(r));
    }

    // ── Link variants ──
    c->addChild(heading("Link Styles"));
    {
        auto r = row(16);
        auto l1 = std::make_unique<Link>("Primary link"); l1->setFont(g_font); l1->setUrl("#"); r->addChild(std::move(l1));
        auto l2 = std::make_unique<Link>("Underline off"); l2->setFont(g_font); l2->setUrl("#"); l2->setUnderline(false); r->addChild(std::move(l2));
        auto l3 = std::make_unique<Link>("Danger link"); l3->setFont(g_font); l3->setUrl("#"); l3->setColorRole(ColorRole::Danger); r->addChild(std::move(l3));
        auto l4 = std::make_unique<Link>("Success link"); l4->setFont(g_font); l4->setUrl("#"); l4->setColorRole(ColorRole::Success); r->addChild(std::move(l4));
        c->addChild(std::move(r));
    }

    // ── Switch variants ──
    c->addChild(heading("Switch Styles"));
    {
        auto r = row(20);
        auto s1 = std::make_unique<Switch>(); s1->setChecked(true);
        auto l1 = std::make_unique<Label>("Default"); l1->setFont(g_font); l1->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s1)); r->addChild(std::move(l1));

        auto s2 = std::make_unique<Switch>(); s2->setChecked(true); s2->setColorRole(ColorRole::Success);
        auto l2 = std::make_unique<Label>("Success"); l2->setFont(g_font); l2->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s2)); r->addChild(std::move(l2));

        auto s3 = std::make_unique<Switch>(); s3->setChecked(false); s3->setColorRole(ColorRole::Danger);
        auto l3 = std::make_unique<Label>("Danger"); l3->setFont(g_font); l3->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s3)); r->addChild(std::move(l3));

        auto s4 = std::make_unique<Switch>(); s4->setChecked(true); s4->setMinSize(56,30);
        auto l4 = std::make_unique<Label>("Large"); l4->setFont(g_font); l4->setColorRole(ColorRole::TextSecondary);
        r->addChild(std::move(s4)); r->addChild(std::move(l4));
        c->addChild(std::move(r));
    }

    // ── InputNumber variants ──
    c->addChild(heading("InputNumber Styles"));
    {
        auto r = row(12);
        auto in1 = std::make_unique<InputNumber>(); in1->setFont(g_font); in1->setValue(42); r->addChild(std::move(in1));
        auto in2 = std::make_unique<InputNumber>(); in2->setFont(g_font); in2->setRange(-50,50); in2->setValue(-10); in2->setStep(5); r->addChild(std::move(in2));
        auto in3 = std::make_unique<InputNumber>(); in3->setFont(g_font); in3->setRange(0,999); in3->setValue(100); r->addChild(std::move(in3));
        c->addChild(std::move(r));
    }

    // ── Image styles ──
    c->addChild(heading("Image Styles (placeholder)"));
    {
        auto r = row(12);
        auto label = std::make_unique<Label>("Image supports setImageRadius / setImageShadow / setScaleMode(Fit/Fill/Stretch)");
        label->setFont(g_font); label->setFontSize(12); label->setColorRole(ColorRole::TextDisabled);
        c->addChild(std::move(label));
    }

    // ── MessageBubble styles ──
    c->addChild(heading("Message Bubble"));
    {
        auto mb1 = std::make_unique<MessageBubble>(); mb1->setFont(g_font);
        mb1->setSender("Alice", "A", Color{0.3f,0.5f,0.9f,1}); mb1->setText("Hello! How are you?"); mb1->setTime("10:30"); mb1->setSide(MessageBubble::Left);
        c->addChild(std::move(mb1));

        auto mb2 = std::make_unique<MessageBubble>(); mb2->setFont(g_font);
        mb2->setSender("You", "Y", Color{0.2f,0.8f,0.4f,1}); mb2->setText("I am good, thanks!"); mb2->setTime("10:31"); mb2->setSide(MessageBubble::Right);
        c->addChild(std::move(mb2));

        auto mb3 = std::make_unique<MessageBubble>(); mb3->setFont(g_font);
        mb3->setSender("Alice", "A", Color{0.3f,0.5f,0.9f,1}); mb3->setText("See you later!"); mb3->setTime("10:32"); mb3->setSide(MessageBubble::Left);
        c->addChild(std::move(mb3));
    }

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Tab 11: Styles 4 — Message, MessageBox, Tooltip
// ============================================================
static std::unique_ptr<Widget> buildStyles4() {
    auto scroll = std::make_unique<ScrollArea>();
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(16));
    c->setSpacing(16);

    // ── MessageBubble variants ──
    c->addChild(heading("Message Bubble Styles"));
    {
        auto mb1 = std::make_unique<MessageBubble>(); mb1->setFont(g_font);
        mb1->setSender("Alice", "A", Color{0.3f,0.5f,0.9f,1}); mb1->setText("Hello! Default style"); mb1->setTime("10:30"); mb1->setSide(MessageBubble::Left);
        c->addChild(std::move(mb1));

        auto mb2 = std::make_unique<MessageBubble>(); mb2->setFont(g_font);
        mb2->setSender("You", "Y", Color{0.2f,0.8f,0.4f,1}); mb2->setText("Sent message - Primary color"); mb2->setTime("10:31"); mb2->setSide(MessageBubble::Right);
        c->addChild(std::move(mb2));

        auto mb3 = std::make_unique<MessageBubble>(); mb3->setFont(g_font);
        mb3->setSender("Bob", "B", Color{1,0.5f,0.2f,1}); mb3->setText("Danger color variant"); mb3->setTime("10:32"); mb3->setSide(MessageBubble::Right); mb3->setColorRole(ColorRole::Danger);
        c->addChild(std::move(mb3));

        auto mb4 = std::make_unique<MessageBubble>(); mb4->setFont(g_font);
        mb4->setSender("Eve", "E", Color{0.3f,0.5f,0.9f,1}); mb4->setText("Card style bubble"); mb4->setTime("10:33"); mb4->setSide(MessageBubble::Left); mb4->setVariant(MessageBubble::Card);
        c->addChild(std::move(mb4));

        auto mb5 = std::make_unique<MessageBubble>(); mb5->setFont(g_font);
        mb5->setSender("You", "Y", Color{0.2f,0.8f,0.4f,1}); mb5->setText("Compact"); mb5->setTime("10:34"); mb5->setSide(MessageBubble::Right); mb5->setVariant(MessageBubble::Compact);
        c->addChild(std::move(mb5));
    }

    // ── MessageBox variants ──
    c->addChild(heading("MessageBox — click to test"));
    {
        auto r = row(8);
        auto b1 = std::make_unique<Button>("Info Dialog"); b1->setFont(g_font); b1->setVariant(Button::Primary); b1->setCompact(true);
        b1->setOnClick([](){
            static auto* mb = new MessageBox(); // must outlive callback
            mb->setTitle("Information"); mb->setMessage("This is an info message box.\nClick OK to dismiss."); mb->setFont(g_font);
            mb->setIcon(IconType::Info); mb->clearButtons(); mb->addButton("OK", [](){ });
            mb->show(App::instance()->root(), 360, 180);
        });
        r->addChild(std::move(b1));

        auto b2 = std::make_unique<Button>("Warning Dialog"); b2->setFont(g_font); b2->setVariant(Button::Danger); b2->setCompact(true);
        b2->setOnClick([](){
            static auto* mb = new MessageBox();
            mb->setTitle("Warning"); mb->setMessage("Are you sure you want to continue?"); mb->setFont(g_font);
            mb->setIcon(IconType::Warning); mb->clearButtons();
            mb->addButton("Cancel", [](){})\
;
            mb->addButton("Continue", [](){})\
;
            mb->show(App::instance()->root(), 360, 180);
        });
        r->addChild(std::move(b2));

        auto b3 = std::make_unique<Button>("Question Dialog"); b3->setFont(g_font); b3->setVariant(Button::Secondary); b3->setCompact(true);
        b3->setOnClick([](){
            static auto* mb = new MessageBox();
            mb->setTitle("Confirm"); mb->setMessage("Do you want to save changes?"); mb->setFont(g_font);
            mb->setIcon(IconType::Question); mb->clearButtons();
            mb->addButton("No", [](){})\
;
            mb->addButton("Yes", [](){})\
;
            mb->show(App::instance()->root(), 380, 180);
        });
        r->addChild(std::move(b3));
        c->addChild(std::move(r));
    }

    // ── Tooltip samples ──
    c->addChild(heading("Tooltip — hover to see"));
    {
        auto r = row(12);
        auto lbl1 = std::make_unique<Label>("Hover me (Top)"); lbl1->setFont(g_font); lbl1->setColorRole(ColorRole::Primary); lbl1->setMinSize(120, 28);
        setTooltip(lbl1.get(), "Tooltip positioned at top");
        r->addChild(std::move(lbl1));

        auto lbl2 = std::make_unique<Label>("Hover me (Right)"); lbl2->setFont(g_font); lbl2->setColorRole(ColorRole::Accent); lbl2->setMinSize(120, 28);
        setTooltip(lbl2.get(), "Tooltip positioned at right with longer text for wrapping test");
        r->addChild(std::move(lbl2));

        auto lbl3 = std::make_unique<Label>("Hover me (Bottom)"); lbl3->setFont(g_font); lbl3->setColorRole(ColorRole::Success); lbl3->setMinSize(120, 28);
        setTooltip(lbl3.get(), "Bottom positioned tooltip");
        r->addChild(std::move(lbl3));
        c->addChild(std::move(r));
    }

    auto note = std::make_unique<Label>("Tooltip positions: Top/Bottom/Left/Right/Auto. Also supports setMaxWidth for text wrapping.");
    note->setFont(g_font); note->setFontSize(11); note->setColorRole(ColorRole::TextDisabled);
    c->addChild(std::move(note));

    scroll->content(std::move(c));
    return scroll;
}

// ============================================================
// Main
// ============================================================
int main() {
    // Initialize themes and transition
    g_lightTheme = Theme::defaultLight();
    g_darkTheme = Theme::defaultDark();
    g_transition.setImmediate(g_darkTheme); // start with dark theme

    App app;
    if (!app.init("ModernUI - Full Demo (All Widgets + Theme Switching)", 1100, 720)) {
        fprintf(stderr, "Failed to initialize\n");
        return -1;
    }
    g_font = app.getFont();

    // Root layout: header + body
    auto root = std::make_unique<Container>(LayoutDirection::Vertical);
    Container* rootPtr = root.get();

    // Add invisible ticker widget to drive theme transition animation each frame
    rootPtr->addChild(std::make_unique<ThemeTicker>());

    // === Header bar ===
    auto topBar = std::make_unique<Container>(LayoutDirection::Horizontal);
    topBar->setPadding(EdgeInsets{10, 16, 10, 16});
    topBar->setMinSize(0, 48);
    // Let theme provide the background color (supports transition animation)
    // topBar background will use m_theme->color(Bg) via Container::paint fallback

    auto logoLabel = std::make_unique<Label>("ModernUI");
    logoLabel->setFont(g_font); logoLabel->setFontSize(18);
    logoLabel->setColorRole(ColorRole::Primary);
    topBar->addChild(std::move(logoLabel));

    auto spacer = std::make_unique<Spacer>(SpaceSize::XL);
    topBar->addChild(std::move(spacer));

    // Theme label
    auto themeLabel = std::make_unique<Label>("Dark Mode");
    themeLabel->setFont(g_font);
    themeLabel->setColorRole(ColorRole::Text);
    Label* themeLabelPtr = themeLabel.get(); // save raw ptr
    topBar->addChild(std::move(themeLabel));

    // Theme switch
    auto themeSwitch = std::make_unique<Switch>();
    themeSwitch->setChecked(true);
    Switch* themeSwitchPtr = themeSwitch.get(); // save raw ptr
    topBar->addChild(std::move(themeSwitch));

    Container* topBarPtr = topBar.get(); // save raw ptr before move
    rootPtr->addChild(std::move(topBar)); // topBar unique_ptr moved, topBarPtr still valid

    // === Body: TabView ===
    auto tabs = std::make_unique<TabView>();
    tabs->setFont(g_font);
    tabs->setMinSize(0, 100);
    // Content border: light line around the content area below tabs
    tabs->setContentBorder(currentTheme().color(ColorRole::Border), 1.0f);
    // Tab bar uses theme color
    tabs->setTabBarColor(currentTheme().color(ColorRole::BgSecondary));
    tabs->addTab("Typography", buildTextShowcase());
    tabs->addTab("Styles",     buildStyles());
    tabs->addTab("Styles 2",   buildStyles2());
    tabs->addTab("Styles 3",   buildStyles3());
    tabs->addTab("Styles 4",   buildStyles4());
    tabs->addTab("Controls",   buildControls());
    tabs->addTab("Inputs",     buildInputs());
    tabs->addTab("Data",       buildData());
    tabs->addTab("Stats",      buildStats());
    tabs->addTab("Calendar",   buildCalendar());
    tabs->addTab("Menu",       buildMenu());

    rootPtr->addChild(std::move(tabs));
    rootPtr->setChildSizing(0, Sizing::fill(1), Sizing::fill(1));

    // Apply initial theme
    applyThemeTo(rootPtr);

    // === Theme toggle callback ===
    // Set this AFTER all widgets are built and raw ptrs are saved.
    // Capture raw pointers by VALUE — they point to stable heap objects.
    themeSwitchPtr->setOnChanged([rootPtr, topBarPtr, themeLabelPtr](bool dark) {
        // Start smooth theme transition (0.5s ease-out)
        g_transition.setTarget(dark ? g_darkTheme : g_lightTheme, 0.5f);
        themeLabelPtr->setText(dark ? "Dark Mode" : "Light Mode");
        showToast(dark ? "Switched to Dark theme" : "Switched to Light theme",
                  IconType::Info, 2.0f);
    });

    app.setRoot(std::move(root));

    printf("\n");
    printf("  === ModernUI Full Demo ===\n");
    printf(" 10 tabs: Typography | Styles | Styles2 | Styles3 | Controls | Inputs | Data | Stats | Calendar | Menu\n");
    printf("  Toggle 'Dark Mode' switch in header to change themes.\n");
    printf("  All widgets are interactive - click, type, and explore!\n");
    printf("\n");

    app.run();
    return 0;
}

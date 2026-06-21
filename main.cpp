#include <cstdio>
#include <memory>
#include <vector>
#include "src/core/App.hpp"
#include "src/core/Types.hpp"
#include "src/widgets/Widget.hpp"
#include "src/widgets/Container.hpp"
#include "src/widgets/Button.hpp"
#include "src/widgets/Label.hpp"
#include "src/widgets/Checkbox.hpp"
#include "src/widgets/RadioButton.hpp"
#include "src/widgets/Slider.hpp"
#include "src/widgets/ScrollBar.hpp"
#include "src/widgets/TextInput.hpp"
#include "src/widgets/TextEdit.hpp"
#include "src/widgets/Dropdown.hpp"
#include "src/widgets/TabView.hpp"
#include "src/widgets/Tooltip.hpp"
#include "src/widgets/Toast.hpp"
#include "src/widgets/Avatar.hpp"
#include "src/widgets/Link.hpp"
#include "src/widgets/ProgressBar.hpp"
#include "src/widgets/Loading.hpp"
#include "src/widgets/Switch.hpp"
#include "src/widgets/InputNumber.hpp"
#include "src/widgets/Select.hpp"
#include "src/widgets/Menu.hpp"
#include "src/widgets/Table.hpp"
#include "src/widgets/Pagination.hpp"
#include "src/widgets/Collapse.hpp"
#include "src/widgets/MessageBox.hpp"
#include "src/widgets/Form.hpp"
#include "src/widgets/Layout.hpp"

using namespace ui;

static ui::Font* g_font = nullptr;
static std::vector<std::shared_ptr<ui::RadioGroup>> g_radioGroups;
static std::unique_ptr<ui::Label> sec(const char* t) {
    auto l = std::make_unique<ui::Label>(t);
    l->setFontSize(12); l->setFont(g_font);
    l->setColorRole(ColorRole::TextSecondary);
    return l;
}
static std::unique_ptr<ui::Container> row() {
    auto r = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    r->setSpacing(8); r->setMinSize(0, 32);
    return r;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); // unbuffered for diagnostics
    ui::App app;
    if (!app.init("ModernUI - All Widgets", 1000, 700)) return -1;
    g_font = app.getFont();

    auto root = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    root->setPadding(ui::EdgeInsets::all(12));
    root->setSpacing(6);
    root->setBackgroundColor(ui::Color::fromHex(0xFF1A1A2E));

    auto tabs = std::make_unique<ui::TabView>();
    tabs->setMinSize(0, 100);

    // ── Tab 1: Basic ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(8);

        c->addChild(sec("Buttons"));
        auto br = row();
        auto b1 = std::make_unique<ui::Button>("Normal");
        b1->setFont(g_font); b1->setCornerRadius(6); br->addChild(std::move(b1));
        auto b2 = std::make_unique<ui::Button>("Gradient");
        b2->setFont(g_font); b2->setCornerRadius(8);
        b2->setGradient({1,0.15f,0.15f,1},{0.15f,0.15f,1,1});
        b2->setHoverColor({0.5f,0.15f,0.85f,1}); b2->setPressColor({0.2f,0.6f,1,1});
        br->addChild(std::move(b2));
        c->addChild(std::move(br));

        c->addChild(sec("TextInput (click focus, right-click menu, drag select)"));
    auto ti = std::make_unique<ui::TextInput>("Click here, or right-click...");
    ti->setFont(g_font); ti->setMinSize(0, 32); c->addChild(std::move(ti));

    c->addChild(sec("Labels"));
        auto lr = row();
        auto l1 = std::make_unique<ui::Label>("Default label");
        l1->setFont(g_font); lr->addChild(std::move(l1));
        auto l2 = std::make_unique<ui::Label>("Colored");
        l2->setFont(g_font); l2->setColorRole(ColorRole::Primary); lr->addChild(std::move(l2));
        c->addChild(std::move(lr));

        c->addChild(sec("Switch"));
        auto sw = std::make_unique<ui::Switch>();
        sw->setOnChanged([](bool v){printf("[Switch] %s\n",v?"ON":"OFF");});
        c->addChild(std::move(sw));

        tabs->addTab("Basic", std::move(c));
    }

    // ── Tab 2: Inputs ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(8);

        c->addChild(sec("TextInput"));
        auto ti = std::make_unique<ui::TextInput>("Type here...");
        ti->setFont(g_font); ti->setMinSize(0, 30); c->addChild(std::move(ti));

        c->addChild(sec("TextEdit (multi-line)"));
        auto te = std::make_unique<ui::TextEdit>("Line 1\nLine 2\nLine 3");
        te->setFont(g_font); te->setMinSize(0, 80); c->addChild(std::move(te));

        c->addChild(sec("InputNumber"));
        auto in = std::make_unique<ui::InputNumber>();
        in->setFont(g_font); in->setRange(0, 100); in->setValue(42);
        c->addChild(std::move(in));

        tabs->addTab("Inputs", std::move(c));
    }

    // ── Tab 3: Selectors ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(8);

        c->addChild(sec("Checkboxes"));
        c->addChild(std::make_unique<ui::Checkbox>("Enable notifications"));
        auto cb = std::make_unique<ui::Checkbox>("Auto-save"); cb->setChecked(true);
        cb->setFont(g_font); c->addChild(std::move(cb));

        c->addChild(sec("RadioButtons"));
        auto gp = std::make_shared<ui::RadioGroup>();
        g_radioGroups.push_back(gp);
        auto r1 = std::make_unique<ui::RadioButton>("Option A", gp.get()); r1->setFont(g_font); c->addChild(std::move(r1));
        auto r2 = std::make_unique<ui::RadioButton>("Option B", gp.get()); r2->setFont(g_font); r2->setSelected(true); c->addChild(std::move(r2));
        auto r3 = std::make_unique<ui::RadioButton>("Option C", gp.get()); r3->setFont(g_font); c->addChild(std::move(r3));

        c->addChild(sec("Dropdown"));
        auto dd = std::make_unique<ui::Dropdown>();
        dd->addItem("Apple"); dd->addItem("Banana"); dd->addItem("Cherry"); dd->addItem("Date");
        c->addChild(std::move(dd));

        tabs->addTab("Selectors", std::move(c));
    }

    // ── Tab 4: Sliders ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(10);

        c->addChild(sec("Slider"));
        auto sl = std::make_unique<ui::Slider>(0, 100, 50);
        sl->setMinSize(0, 28); c->addChild(std::move(sl));

        c->addChild(sec("ProgressBar"));
        auto pb = std::make_unique<ui::ProgressBar>();
        pb->setValue(0.65f); pb->setMinSize(0, 8); c->addChild(std::move(pb));

        c->addChild(sec("ScrollBar (standalone)"));
        auto sb = std::make_unique<ui::ScrollBar>(ui::ScrollBar::Horizontal);
        sb->setRange(500, 200); sb->setMinSize(200, 12); c->addChild(std::move(sb));

        c->addChild(sec("Loading"));
        c->addChild(std::make_unique<ui::LoadingSpinner>());

        tabs->addTab("Sliders", std::move(c));
    }

    // ── Tab 5: Data ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(8);

        c->addChild(sec("Table"));
        auto tbl = std::make_unique<ui::Table>(); tbl->setFont(g_font);
        tbl->setColumns({"Name", "Status", "Role"}, {100, 70, 100});
        tbl->addRow({"Alice", "Active", "Admin"});
        tbl->addRow({"Bob", "Inactive", "User"});
        tbl->addRow({"Charlie", "Active", "Editor"});
        c->addChild(std::move(tbl));

        c->addChild(sec("Pagination"));
        auto pg = std::make_unique<ui::Pagination>();
        pg->setFont(g_font); pg->setTotalPages(5);
        c->addChild(std::move(pg));

        c->addChild(sec("Collapse"));
        auto col = std::make_unique<ui::CollapsePanel>("Details");
        col->setFont(g_font);
        auto cc = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        cc->setPadding(ui::EdgeInsets::all(6));
        cc->addChild(std::make_unique<ui::Label>("Item 1")); cc->addChild(std::make_unique<ui::Label>("Item 2"));
        col->setContent(std::move(cc));
        c->addChild(std::move(col));

        tabs->addTab("Data", std::move(c));
    }

    // ── Tab 6: Menu ──
    {
        auto mc = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        auto nav = std::make_unique<ui::NavMenu>(); nav->setFont(g_font); nav->setMinSize(200, 100);
        nav->setItems({
            {"Dashboard", ui::IconType::Home, [](){printf("[Nav] Dashboard\n");}, true, {}},
            {"Analytics", ui::IconType::Star, nullptr, true, {
                {"Overview", ui::IconType::None, [](){printf("[Nav] Overview\n");}, true, {}},
                {"Reports", ui::IconType::None, [](){printf("[Nav] Reports\n");}, true, {}},
            }},
            {"Users", ui::IconType::User, nullptr, true, {
                {"Profile", ui::IconType::None, [](){}, true, {}},
                {"Security", ui::IconType::Lock, [](){}, true, {}},
            }},
            {"Settings", ui::IconType::Settings, [](){}, true, {}},
        });
        mc->addChild(std::move(nav));
        tabs->addTab("Menu", std::move(mc));
    }

    // ── Tab 7: Select ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(12);

        c->addChild(sec("Color"));
        auto s1 = std::make_unique<ui::Select>(); s1->setFont(g_font); s1->setMinSize(160, 100);
        s1->addItem("Red"); s1->addItem("Green"); s1->addItem("Blue"); s1->addItem("Yellow");
        c->addChild(std::move(s1));

        c->addChild(sec("Size"));
        auto s2 = std::make_unique<ui::Select>(); s2->setFont(g_font); s2->setMinSize(160, 80);
        s2->addItem("Small"); s2->addItem("Medium"); s2->addItem("Large"); s2->addItem("X-Large");
        s2->setSelectedIndex(1);
        c->addChild(std::move(s2));

        c->addChild(sec("Theme"));
        auto s3 = std::make_unique<ui::Select>(); s3->setFont(g_font); s3->setMinSize(160, 60);
        s3->addItem("Light"); s3->addItem("Dark"); s3->addItem("Auto");
        s3->setSelectedIndex(2);
        c->addChild(std::move(s3));

        tabs->addTab("Select", std::move(c));
    }

    // ── Tab 8: Misc ──
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(10)); c->setSpacing(8);

        c->addChild(sec("Link"));
        auto link = std::make_unique<ui::Link>("Visit GitHub");
        link->setFont(g_font); link->setUrl("https://github.com");
        c->addChild(std::move(link));

        c->addChild(sec("Avatar"));
        auto av = std::make_unique<ui::Avatar>();
        av->setName("Alice"); av->setSize(40); av->setColorRole(ColorRole::Danger);
        c->addChild(std::move(av));

        c->addChild(sec("Form"));
        auto fm = std::make_unique<ui::Form>();
        fm->setFont(g_font);
        fm->addField("Username", std::make_unique<ui::TextInput>());
        fm->addField("Age", std::make_unique<ui::InputNumber>());
        c->addChild(std::move(fm));

        c->addChild(sec("Toast"));
        auto tb = std::make_unique<ui::Button>("Show Toast");
        tb->setFont(g_font); tb->setCornerRadius(6);
        tb->setOnClick([](){ui::showToast("Operation completed!", ui::IconType::Check);});
        c->addChild(std::move(tb));

        tabs->addTab("Misc", std::move(c));
    }

    root->addChild(std::move(tabs));
    root->setChildSizing(0, ui::Sizing::fill(1), ui::Sizing::fill(1));
    app.setRoot(std::move(root));

    printf("All widgets test. 8 tabs, no overflow = no scrollbars.\n");
    app.run();
    return 0;
}

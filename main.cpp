#include <cstdio>
#include <memory>

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
#include "src/widgets/MessageBox.hpp"
#include "src/widgets/Avatar.hpp"
#include "src/widgets/Message.hpp"
#include "src/widgets/Toast.hpp"
#include "src/widgets/Border.hpp"
#include "src/widgets/Link.hpp"
#include "src/widgets/ProgressBar.hpp"
#include "src/widgets/Loading.hpp"
#include "src/widgets/Pagination.hpp"
#include "src/widgets/Collapse.hpp"
#include "src/widgets/Upload.hpp"
#include "src/widgets/Table.hpp"
#include "src/widgets/FileBrowser.hpp"
#include "src/widgets/Form.hpp"
#include "src/widgets/InputNumber.hpp"
#include "src/widgets/FlexLayout.hpp"
#include "src/widgets/GridLayout.hpp"
#include "src/widgets/Space.hpp"
#include "src/widgets/Switch.hpp"
#include "src/widgets/Menu.hpp"
#include "src/widgets/LayoutContainer.hpp"
#include "src/widgets/Select.hpp"
#include "src/widgets/Layout.hpp"

static ui::Font* g_font = nullptr;
static std::unique_ptr<ui::Label> section(const char* text) {
    auto l = std::make_unique<ui::Label>(text);
    l->setFontSize(13);
    l->setFont(g_font);
    l->setColor(ui::Color::fromHex(0xFF888899));
    return l;
}
static std::unique_ptr<ui::Container> row() {
    auto r = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    r->setSpacing(10);
    r->setMinSize(0, 32);
    return r;
}

int main() {
    ui::App app;
    if (!app.init("ModernUI - All Widgets", 1280, 800)) { return -1; }
    g_font = app.getFont();

    auto root = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    root->setPadding(ui::EdgeInsets::all(16));
    root->setSpacing(8);
    root->setResponsive(true); // margin/spacing scale with window width
    root->setBackgroundColor(ui::Color::fromHex(0xFF1A1A2E));
    root->setFrosted(true);

    auto titleBar = row();
    auto title = std::make_unique<ui::Label>("ModernUI Widget Test");
    title->setFontSize(20);
    title->setColor(ui::Color::fromHex(0xFFFFFFFF));
    titleBar->addChild(std::move(title));
    root->addChild(std::move(titleBar));

    auto tabs = std::make_unique<ui::TabView>();
    tabs->setMinSize(0, 100);
    // TabView fills remaining space after title + bottom bar

    // Tab 1: Buttons & Labels
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(8);
        c->addChild(section("Buttons"));
        auto br = row();
        auto b1 = std::make_unique<ui::Button>("Normal"); b1->setFont(g_font);
    ui::setTooltip(b1.get(), "A normal button"); br->addChild(std::move(b1));
        auto b2 = std::make_unique<ui::Button>("Gradient"); b2->setFont(g_font);
        b2->setGradient({0.9f,0.2f,0.2f,1},{0.2f,0.2f,0.9f,1});
        b2->setHoverColor({0.5f,0.15f,0.8f,1}); b2->setPressColor({0.2f,0.6f,1.0f,1}); b2->setCornerRadius(10);
        b2->setShadow({2,3}, 8, {0,0,0,0.3f});
        br->addChild(std::move(b2));
        c->addChild(std::move(br));
        c->addChild(section("Labels"));
        auto lr = row();
        auto l1 = std::make_unique<ui::Label>("Normal"); lr->addChild(std::move(l1));
        auto l2 = std::make_unique<ui::Label>("Blue"); l2->setColor(ui::Color::fromHex(0xFF3B82F6)); lr->addChild(std::move(l2));
        c->addChild(std::move(lr));
        tabs->addTab("Buttons+Labels", std::move(c));
    }

    // Tab 2: Inputs
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(8);
        c->addChild(section("TextInput"));
        auto ti = std::make_unique<ui::TextInput>("Type here...");
        ti->setMinSize(0, 32); ti->setFont(app.getFont());
        c->addChild(std::move(ti));
        c->addChild(section("TextEdit"));
        auto te = std::make_unique<ui::TextEdit>("Line one\nLine two\nLine three");
        te->setMinSize(0, 150); te->setFont(app.getFont());
        c->addChild(std::move(te));
        c->addChild(section("Dropdown"));
        auto dd = std::make_unique<ui::Dropdown>();
        dd->addItem("Apple"); dd->addItem("Banana"); dd->addItem("Cherry");
        dd->addItem("Dragon Fruit"); dd->setSelectedIndex(1);
        c->addChild(std::move(dd));
        tabs->addTab("Inputs", std::move(c));
    }

    // Tab 3: Selectors
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(8);
        c->addChild(section("Checkboxes"));
        auto cb1 = std::make_unique<ui::Checkbox>("Enable notifications"); cb1->setFont(g_font); c->addChild(std::move(cb1));
        auto cb2 = std::make_unique<ui::Checkbox>("Auto-update"); cb2->setChecked(true); cb2->setFont(g_font); c->addChild(std::move(cb2));
        c->addChild(section("Checkboxes2"));
        auto cb3 = std::make_unique<ui::Checkbox>("Dark mode"); cb3->setFont(g_font); c->addChild(std::move(cb3));
        auto cb4 = std::make_unique<ui::Checkbox>("Auto-save"); cb4->setFont(g_font); c->addChild(std::move(cb4));

        c->addChild(section("RadioButtons"));
        auto* gp = new ui::RadioGroup(); // kept alive for demo lifetime
        auto rb1 = std::make_unique<ui::RadioButton>("Small", gp); rb1->setFont(g_font); c->addChild(std::move(rb1));
        auto rb2 = std::make_unique<ui::RadioButton>("Medium", gp); rb2->setFont(g_font); rb2->setSelected(true); c->addChild(std::move(rb2));
        auto rb3 = std::make_unique<ui::RadioButton>("Large", gp); rb3->setFont(g_font); c->addChild(std::move(rb3));
        tabs->addTab("Selectors", std::move(c));
    }

    // Tab 4: Sliders
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(12);
        c->addChild(section("Volume"));
        auto s1 = std::make_unique<ui::Slider>(0,100,75); s1->setMinSize(0,30); c->addChild(std::move(s1));
        c->addChild(section("ScrollBar"));
        auto sb = std::make_unique<ui::ScrollBar>(ui::ScrollBar::Horizontal);
        sb->setRange(800,300); sb->setMinSize(200,14); c->addChild(std::move(sb));
        tabs->addTab("Sliders", std::move(c));
    }

    // Tab 5: Chat Messages
    {
        using Msg = std::tuple<std::string,std::string,ui::Color,ui::MessageBubble::Side,std::string>;
        auto chat = ui::MessageBubble::createChatList(g_font, {
            Msg{"Alice", "AL", ui::Color{0.9f,0.3f,0.3f,1}, ui::MessageBubble::Left, "Hey! How is the new UI library coming along?"},
            Msg{"You", "YO", ui::Color{0.3f,0.5f,0.9f,1}, ui::MessageBubble::Right, "Great! It supports gradients, shadows, rounded corners, and frosted glass now."},
            Msg{"Alice", "AL", ui::Color{0.9f,0.3f,0.3f,1}, ui::MessageBubble::Left, "That sounds amazing! Can I see a demo?"},
            Msg{"You", "YO", ui::Color{0.3f,0.5f,0.9f,1}, ui::MessageBubble::Right, "Sure! This is the demo right here. Everything you see is rendered with OpenGL."},
        });
        tabs->addTab("Chat", std::move(chat));
    }

    // Tab 6: More Components
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(10);

        // Link with real URL (Navigate mode opens in browser)
        auto link = std::make_unique<ui::Link>("Visit GitHub");
        link->setFont(g_font);
        link->setUrl("https://github.com");
        c->addChild(std::move(link));

        auto link2 = std::make_unique<ui::Link>("HTTP GET test (check console)");
        link2->setFont(g_font); link2->setMode(ui::Link::HttpGet);
        link2->setUrl("http://httpbin.org/get");
        c->addChild(std::move(link2));

        // ProgressBar
        auto pb = std::make_unique<ui::ProgressBar>();
        pb->setValue(0.65f); pb->setMinSize(0, 8);
        c->addChild(std::move(pb));

        // Loading spinner
        auto spin = std::make_unique<ui::LoadingSpinner>();
        spin->setMinSize(24, 24);
        c->addChild(std::move(spin));

        // Pagination
        auto pag = std::make_unique<ui::Pagination>();
        pag->setFont(g_font); pag->setTotalPages(5);
        pag->setOnPageChanged([](int p) { printf("[Page] %d\n", p); });
        c->addChild(std::move(pag));

        // Upload (opens FileBrowser overlay)
        auto upl = std::make_unique<ui::Upload>(); upl->setFont(g_font);
        upl->setOpenFilePicker([&app](ui::Upload* u) {
            auto* fb = new ui::FileBrowser();
            fb->setFont(g_font); fb->setMinSize(500, 300);
            fb->setRootPath("C:/");
            fb->setOnFileSelected([&app, u](const std::string& path) {
                u->setSelectedFile(path);
                app.setOverlay(nullptr);
                printf("[Upload] Selected: %s\n", path.c_str());
            });
            fb->setOnClose([&app]() { app.setOverlay(nullptr); });
            fb->show(app.root());
            app.setOverlay(fb);
        });
        c->addChild(std::move(upl));

        // Collapse
        auto col = std::make_unique<ui::CollapsePanel>("Advanced Settings");
        col->setFont(g_font);
        auto colContent = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        colContent->setPadding(ui::EdgeInsets::all(8));
        colContent->addChild(std::make_unique<ui::Label>("Option 1: Enabled"));
        colContent->addChild(std::make_unique<ui::Label>("Option 2: Auto"));
        col->setContent(std::move(colContent));
        c->addChild(std::move(col));

        // Table
        auto tbl = std::make_unique<ui::Table>(); tbl->setFont(g_font);
        tbl->setColumns({"Name", "Status", "Role"}, {120, 80, 120});
        tbl->addRow({"Alice", "Active", "Admin"});
        tbl->addRow({"Bob", "Inactive", "User"});
        tbl->addRow({"Charlie", "Active", "Editor"});
        c->addChild(std::move(tbl));

        tabs->addTab("More", std::move(c));
    }

    // Tab 7: File Browser
    {
        auto fb = std::make_unique<ui::FileBrowser>();
        fb->setFont(g_font); fb->setMinSize(0, 250);
        fb->setRootPath("C:/");
        fb->setFilter(".cpp,.hpp,.h,.txt,.json");
        fb->setOnFileSelected([](const std::string& path) { printf("[FileBrowser] Selected: %s\n", path.c_str()); });
        tabs->addTab("Files", std::move(fb));
    }

    // Tab 8: Form + Avatar + InputNumber
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(12);

        // Avatar row
        auto avRow = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        avRow->setSpacing(16); avRow->setMinSize(0, 50);
        auto av1 = std::make_unique<ui::Avatar>(); av1->setName("Alice"); av1->setSize(44);
        av1->setColor(ui::Color{0.9f,0.3f,0.3f,1}); avRow->addChild(std::move(av1));
        auto av2 = std::make_unique<ui::Avatar>(); av2->setName("Bob Chen"); av2->setSize(44);
        av2->setColor(ui::Color{0.3f,0.7f,0.3f,1}); avRow->addChild(std::move(av2));
        auto av3 = std::make_unique<ui::Avatar>(); av3->setName("You"); av3->setSize(44);
        av3->setColor(ui::Color{0.3f,0.5f,0.9f,1}); avRow->addChild(std::move(av3));
        c->addChild(std::move(avRow));

        // Form
        auto form = std::make_unique<ui::Form>();
        form->setFont(g_font);
        form->addField("Username", std::make_unique<ui::TextInput>("Enter username"));
        form->addField("Age", std::make_unique<ui::InputNumber>());
        auto pwd = std::make_unique<ui::TextInput>("Password"); // placeholder only, no real masking
        form->addField("Password", std::move(pwd));
        c->addChild(std::move(form));

        tabs->addTab("Form", std::move(c));
    }

    // Tab 9: Layouts (Flex + Grid + Responsive)
    {
        auto c = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
        c->setPadding(ui::EdgeInsets::all(12)); c->setSpacing(12);

        c->addChild(section("FlexLayout (grow ratios)"));
        auto flex = std::make_unique<ui::FlexLayout>(ui::LayoutDirection::Horizontal);
        flex->setSpacing(8); flex->setMinSize(0, 40);
        flex->setItemFlex(0, 1, 1); flex->setItemFlex(1, 2, 1); flex->setItemFlex(2, 1, 1);
        auto fb1 = std::make_unique<ui::Button>("1x"); fb1->setCornerRadius(6);
        flex->addChild(std::move(fb1));
        auto fb2 = std::make_unique<ui::Button>("2x"); fb2->setCornerRadius(6);
        flex->addChild(std::move(fb2));
        auto fb3 = std::make_unique<ui::Button>("1x"); fb3->setCornerRadius(6);
        flex->addChild(std::move(fb3));
        c->addChild(std::move(flex));

        c->addChild(section("GridLayout (2 col, spans)"));
        auto grid = std::make_unique<ui::GridLayout>(); grid->setColumns(2);
        grid->setSpacing(6, 6);
        grid->addCell(0, 0, std::make_unique<ui::Button>("Cell 0,0"), 1, 1);
        grid->addCell(0, 1, std::make_unique<ui::Button>("Cell 0,1"), 1, 1);
        grid->addCell(1, 0, std::make_unique<ui::Button>("Span 2 cols"), 2, 2);
        c->addChild(std::move(grid));

        c->addChild(section("ResponsiveLayout"));
        auto resp = std::make_unique<ui::ResponsiveLayout>();
        resp->setBreakpoint(ui::ResponsiveSize::XS, ui::LayoutDirection::Vertical, 4, ui::EdgeInsets::all(4));
        resp->setBreakpoint(ui::ResponsiveSize::MD, ui::LayoutDirection::Horizontal, 8, ui::EdgeInsets::all(8));
        resp->addChild(std::make_unique<ui::Button>("Auto"));
        resp->addChild(std::make_unique<ui::Button>("Layout"));
        c->addChild(std::move(resp));

        // Spacer and Divider demo
        c->addChild(section("Align: SpaceBetween"));
        auto sbRow = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        sbRow->setAlign(ui::Container::Align::SpaceBetween); sbRow->setMinSize(0, 32);
        sbRow->addChild(std::make_unique<ui::Button>("A"));
        sbRow->addChild(std::make_unique<ui::Button>("B"));
        sbRow->addChild(std::make_unique<ui::Button>("C"));
        c->addChild(std::move(sbRow));

        c->addChild(section("Align: SpaceAround"));
        auto saRow = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        saRow->setAlign(ui::Container::Align::SpaceAround); saRow->setMinSize(0, 32);
        saRow->addChild(std::make_unique<ui::Button>("1"));
        saRow->addChild(std::make_unique<ui::Button>("2"));
        c->addChild(std::move(saRow));

        c->addChild(section("Divider"));
        c->addChild(ui::divider(ui::Divider::Horizontal, ui::DividerStyle::Dashed));

        // Switch
        auto swRow = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        swRow->setSpacing(8); swRow->setMinSize(0, 30);
        auto sw = std::make_unique<ui::Switch>();
        sw->setId("test-switch");
        sw->setOnChanged([](bool v) { printf("[Switch] %s\n", v ? "ON" : "OFF"); });
        swRow->addChild(std::move(sw));
        auto swLabel = std::make_unique<ui::Label>("Toggle"); swLabel->setFont(g_font);
        swRow->addChild(std::move(swLabel));
        c->addChild(std::move(swRow));

        // Multi-level NavMenu test
        c->addChild(section("NavMenu (sidebar navigation)"));
        auto nav = std::make_unique<ui::NavMenu>(); nav->setFont(g_font); nav->setMinSize(200, 180);
        nav->addItem("Dashboard", ui::IconType::Home, []() { printf("[Nav] Dashboard\n"); });
        nav->addSubMenu("Components", ui::IconType::Settings, {
            {"Buttons", ui::IconType::None, []() { printf("[Nav] Buttons\n"); }, true, {}},
            {"Inputs", ui::IconType::None, []() { printf("[Nav] Inputs\n"); }, true, {}},
            {"Data Display", ui::IconType::None, []() { printf("[Nav] Data\n"); }, true, {}},
        });
        nav->addSubMenu("Layout", ui::IconType::Folder, {
            {"Flex", ui::IconType::None, []() {}, true, {}},
            {"Grid", ui::IconType::None, []() {}, true, {}},
        });
        nav->addItem("Settings", ui::IconType::Settings, []() { printf("[Nav] Settings\n"); });
        c->addChild(std::move(nav));

        // Select test
        c->addChild(section("Select (list picker)"));
        auto sel = std::make_unique<ui::Select>(); sel->setFont(g_font); sel->setMinSize(160, 80);
        sel->addItem("Small"); sel->addItem("Medium"); sel->addItem("Large");
        sel->addItem("Extra Large"); sel->setSelectedIndex(1);
        sel->setOnChanged([](size_t i, const std::string& s) { printf("[Select] %zu:'%s'\n", i, s.c_str()); });
        c->addChild(std::move(sel));

        // Avatar test row
        c->addChild(section("Avatars"));
        auto avRow2 = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
        avRow2->setSpacing(12); avRow2->setMinSize(0, 52);
        struct AvInfo { std::string name; ui::Color color; };
        AvInfo avs[] = {{"Alice", {0.9f,0.2f,0.2f,1}}, {"Bob Chen",{0.2f,0.7f,0.2f,1}},
                        {"Charlie",{0.8f,0.6f,0.1f,1}}, {"Diana",{0.6f,0.2f,0.8f,1}},
                        {"Eve",{0.2f,0.5f,0.8f,1}}};
        for (auto& a : avs) {
            auto av = std::make_unique<ui::Avatar>(); av->setName(a.name);
            av->setSize(40); av->setColor(a.color); avRow2->addChild(std::move(av));
        }
        c->addChild(std::move(avRow2));

        tabs->addTab("Layouts", std::move(c));
    }

    root->addChild(std::move(tabs));

    // Button row: Show MessageBox + Quit
    auto bottomRow = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    bottomRow->setSpacing(10); bottomRow->setMinSize(0, 40);

    auto msgBtn = std::make_unique<ui::Button>("Show Message");
    msgBtn->setFont(g_font);
    msgBtn->setCornerRadius(8);
    msgBtn->setGradient({0.2f,0.5f,0.9f,1},{0.15f,0.3f,0.7f,1});
    msgBtn->setHoverColor({0.3f,0.6f,1.0f,1});
    msgBtn->setPressColor({0.15f,0.4f,0.8f,1});
    msgBtn->setShadow({2,3}, 8, {0,0,0,0.3f});
    ui::setTooltip(msgBtn.get(), "Open a message box");
    auto* msgBoxPtr = new ui::MessageBox(); // single instance for demo
    auto toastBtn = std::make_unique<ui::Button>("Toast");
    toastBtn->setFont(g_font); toastBtn->setCornerRadius(8);
    toastBtn->setGradient({0.2f,0.7f,0.3f,1},{0.15f,0.5f,0.2f,1});
    toastBtn->setHoverColor({0.3f,0.8f,0.4f,1}); toastBtn->setPressColor({0.15f,0.6f,0.25f,1});
    toastBtn->setOnClick([]() { ui::showToast("Operation completed successfully!", ui::IconType::Check, 2.5f); });
    ui::setTooltip(toastBtn.get(), "Show a toast notification");
    bottomRow->addChild(std::move(toastBtn));

    msgBtn->setOnClick([&app, msgBoxPtr]() {
        if (msgBoxPtr->isOpen()) return;
        msgBoxPtr->setFont(g_font);
        msgBoxPtr->setIcon(ui::IconType::Warning);
        msgBoxPtr->setTitle("Save Changes");
        msgBoxPtr->setMessage("Do you want to save your changes before closing?\nUnsaved changes will be lost.");
        msgBoxPtr->clearButtons();
        msgBoxPtr->addButton("Save", [&app, msgBoxPtr]() { printf("[MSG] Save\n"); app.setOverlay(nullptr); });
        msgBoxPtr->addButton("Discard", [&app, msgBoxPtr]() { printf("[MSG] Discard\n"); app.setOverlay(nullptr); });
        msgBoxPtr->addButton("Cancel", [&app, msgBoxPtr]() { printf("[MSG] Cancel\n"); app.setOverlay(nullptr); });
        msgBoxPtr->show(app.root());
        app.setOverlay(msgBoxPtr);
    });
    bottomRow->addChild(std::move(msgBtn));

    auto quitBtn = std::make_unique<ui::Button>("Quit");
    quitBtn->setGradient({0.9f,0.2f,0.2f,1},{0.6f,0.1f,0.1f,1});
    quitBtn->setHoverColor({0.7f,0.15f,0.15f,1}); quitBtn->setPressColor({0.5f,0.1f,0.1f,1});
    quitBtn->setCornerRadius(8);
    ui::setTooltip(quitBtn.get(), "Exit the application");
    quitBtn->setShadow({2,3}, 8, {0,0,0,0.35f});
    quitBtn->setOnClick([&app](){app.quit();});
    bottomRow->addChild(std::move(quitBtn));
    // Menu tab: NavMenu + Select
    // Menu tab: NavMenu sidebar + Select content


    root->addChild(std::move(bottomRow));
    root->setChildSizing(1, ui::Sizing::fill(1), ui::Sizing::fill(1));
    app.setRoot(std::move(root));

    // Animation tests — access via app.root()
    ui::Widget* r = app.root();
    if (r) {
        if (r->childCount() > 0) r->childAt(0)->fadeIn(0.5f); // title fade
        if (r->childCount() > 2) r->childAt(r->childCount()-1)->slideIn({0, 60}, 0.4f); // quit slide up
        if (r->childCount() > 1) r->childAt(1)->fadeIn(0.4f); // tabs fade
    }
    // Slide buttons from left
    ui::Widget* tv = app.root();
    if (tv && tv->childCount() > 1) tv = tv->childAt(1); // TabView
    if (tv && tv->childCount() > 0) {
        ui::Widget* t0 = tv->childAt(0);
        if (t0 && t0->childCount() > 1) {
            ui::Widget* br = t0->childAt(1); // button row
            for (size_t i = 0; i < br->childCount(); ++i)
                br->childAt(i)->slideIn({-80 - i*30.0f, 0}, 0.3f + i*0.1f);
        }
    }

    printf("All widgets test with animations. Tab to switch.\n");
    app.run();
    return 0;
}

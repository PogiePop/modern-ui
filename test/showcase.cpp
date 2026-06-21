// ModernUI — Optimized Showcase
#include <cstdio>
#include <memory>
#include <vector>
#include "core/App.hpp"
#include "core/Types.hpp"
#include "res/Style.hpp"
#include "widgets/Container.hpp"
#include "widgets/Button.hpp"
#include "widgets/Label.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/RadioButton.hpp"
#include "widgets/Slider.hpp"
#include "widgets/Switch.hpp"
#include "widgets/Select.hpp"
#include "widgets/Menu.hpp"
#include "widgets/Avatar.hpp"
#include "widgets/ProgressBar.hpp"
#include "widgets/TabView.hpp"
#include "widgets/Toast.hpp"

static ui::Font* g_font = nullptr;
static std::vector<std::shared_ptr<ui::RadioGroup>> g_radioGroups;
using namespace ui;

static std::unique_ptr<Label> sec(const char* t) {
    auto l = std::make_unique<Label>(t); l->setFontSize(11); l->setFont(g_font); l->setColor(style::textDim()); return l;
}
static std::unique_ptr<Container> row() { auto r=std::make_unique<Container>(LayoutDirection::Horizontal); r->setSpacing(8); r->setMinSize(0,34); return r; }

int main() {
    App app;
    if (!app.init("ModernUI — Showcase", 960, 640)) return -1;
    g_font = app.getFont();

    auto root = std::make_unique<Container>(LayoutDirection::Vertical);
    root->setBackgroundColor(style::bg());

    // Top bar
    auto top = std::make_unique<Container>(LayoutDirection::Horizontal);
    top->setPadding(EdgeInsets{12,16,12,16}); top->setMinSize(0,44);
    top->setBackgroundColor(style::surface());
    auto logo = std::make_unique<Label>("ModernUI"); logo->setFontSize(16); logo->setFont(g_font); logo->setColor(style::primary());
    top->addChild(std::move(logo));
    root->addChild(std::move(top));

    auto body = std::make_unique<Container>(LayoutDirection::Horizontal);
    body->setMinSize(0, 100);

    // Sidebar
    auto side = std::make_unique<Container>(LayoutDirection::Vertical);
    side->setMinSize(200, 0); side->setPadding(style::padSm());
    side->setBackgroundColor(style::surface());

    auto av = std::make_unique<Avatar>(); av->setName("User"); av->setSize(36); av->setColor(style::primary());
    side->addChild(std::move(av));

    auto nav = std::make_unique<NavMenu>(); nav->setFont(g_font); nav->setMinSize(180, 100);
    nav->setItems({
        {"Dashboard", IconType::Home, nullptr, true, {}},
        {"Projects", IconType::Folder, nullptr, true, {
            {"Web App", IconType::None, nullptr, true, {}},
            {"Mobile", IconType::None, nullptr, true, {}},
        }},
        {"Settings", IconType::Settings, nullptr, true, {}},
    });
    side->addChild(std::move(nav));
    body->addChild(std::move(side));

    // Main content with tabs
    auto main = std::make_unique<TabView>(); main->setMinSize(0, 100);

    // Tab: Dashboard
    {
        auto c = std::make_unique<Container>(LayoutDirection::Vertical);
        c->setPadding(style::pad()); c->setSpacing(style::gap());

        c->addChild(sec("Quick Actions"));
        auto ar = row();
        auto b1 = std::make_unique<Button>("New Project"); b1->setFont(g_font); b1->setCornerRadius(6);
        b1->setGradient(style::primary(), Color{0.15f,0.3f,0.6f,1}); b1->setHoverColor(style::primaryHover());
        b1->setShadow({0,2},6,{0,0,0,0.25f}); ar->addChild(std::move(b1));
        auto b2 = std::make_unique<Button>("Import"); b2->setFont(g_font); b2->setCornerRadius(6);
        b2->setGradient(style::surfaceHigh(), style::surfaceHigh()); b2->setHoverColor(Color::fromHex(0xFF2E2E4A));
        ar->addChild(std::move(b2));
        c->addChild(std::move(ar));

        c->addChild(sec("Status"));
        auto sr = row();
        auto sw = std::make_unique<Switch>(); sr->addChild(std::move(sw));
        auto sl = std::make_unique<Label>("Active"); sl->setFont(g_font); sl->setColor(style::textDim());
        sr->addChild(std::move(sl));
        c->addChild(std::move(sr));

        c->addChild(sec("Progress"));
        auto pb = std::make_unique<ProgressBar>(); pb->setValue(0.72f); pb->setMinSize(0,6);
        c->addChild(std::move(pb));

        main->addTab("Dashboard", std::move(c));
    }

    // Tab: Inputs
    {
        auto c = std::make_unique<Container>(LayoutDirection::Vertical);
        c->setPadding(style::pad()); c->setSpacing(style::gap());

        c->addChild(sec("Text Fields"));
        auto t1 = std::make_unique<TextInput>("First name"); t1->setFont(g_font); t1->setMinSize(0,32); c->addChild(std::move(t1));
        auto t2 = std::make_unique<TextInput>("Email address"); t2->setFont(g_font); t2->setMinSize(0,32); c->addChild(std::move(t2));

        c->addChild(sec("Checkboxes"));
        auto c1 = std::make_unique<Checkbox>("Accept terms"); c1->setFont(g_font); c->addChild(std::move(c1));
        auto c2 = std::make_unique<Checkbox>("Subscribe to newsletter"); c2->setFont(g_font); c2->setChecked(true); c->addChild(std::move(c2));

        c->addChild(sec("Select"));
        auto s = std::make_unique<Select>(); s->setFont(g_font); s->setMinSize(0,80);
        s->addItem("Option 1"); s->addItem("Option 2"); s->addItem("Option 3");
        c->addChild(std::move(s));

        main->addTab("Inputs", std::move(c));
    }

    // Tab: Controls
    {
        auto c = std::make_unique<Container>(LayoutDirection::Vertical);
        c->setPadding(style::pad()); c->setSpacing(style::gap());

        c->addChild(sec("Sliders"));
        auto s1 = std::make_unique<Slider>(0,100,60); s1->setMinSize(0,24); c->addChild(std::move(s1));
        auto s2 = std::make_unique<Slider>(0,100,35); s2->setMinSize(0,24); c->addChild(std::move(s2));

        c->addChild(sec("Radio"));
        auto gp = std::make_shared<RadioGroup>();
        g_radioGroups.push_back(gp);
        auto r1 = std::make_unique<RadioButton>("Light", gp.get()); r1->setFont(g_font); c->addChild(std::move(r1));
        auto r2 = std::make_unique<RadioButton>("Dark", gp.get()); r2->setFont(g_font); r2->setSelected(true); c->addChild(std::move(r2));
        auto r3 = std::make_unique<RadioButton>("System", gp.get()); r3->setFont(g_font); c->addChild(std::move(r3));

        main->addTab("Controls", std::move(c));
    }

    body->addChild(std::move(main));
    body->setChildSizing(1, Sizing::fill(1), Sizing::fill(1));
    root->addChild(std::move(body));
    root->setChildSizing(1, Sizing::fill(1), Sizing::fill(1));

    app.setRoot(std::move(root));

    ui::showToast("Welcome to ModernUI!", IconType::Info, 3.0f);
    app.run();
    return 0;
}

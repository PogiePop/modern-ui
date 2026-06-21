// ModernUI — User Profile Demo
#include <cstdio>
#include <memory>
#include "core/App.hpp"
#include "core/Types.hpp"
#include "widgets/Container.hpp"
#include "widgets/Button.hpp"
#include "widgets/Label.hpp"
#include "widgets/Avatar.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/Select.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/Form.hpp"
#include "widgets/Toast.hpp"

using namespace ui;

int main() {
    ui::App app;
    if (!app.init("User Profile - ModernUI", 500, 560)) return -1;
    auto* font = app.getFont();

    auto root = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    root->setPadding(ui::EdgeInsets::all(24));
    root->setSpacing(12);
    root->setBackgroundColor(ui::Color::fromHex(0xFF1A1A2E));

    // Header
    auto hdr = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    hdr->setSpacing(16); hdr->setMinSize(0, 56);

    auto av = std::make_unique<ui::Avatar>();
    av->setName("Alice Johnson"); av->setSize(48);
    av->setColorRole(ColorRole::Primary);
    hdr->addChild(std::move(av));

    auto hinfo = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    hinfo->setSpacing(2);
    auto nm = std::make_unique<ui::Label>("Alice Johnson");
    nm->setFontSize(18); nm->setFont(font); nm->setColorRole(ColorRole::Text);
    hinfo->addChild(std::move(nm));
    auto em = std::make_unique<ui::Label>("alice@example.com");
    em->setFontSize(12); em->setFont(font); em->setColorRole(ColorRole::TextSecondary);
    hinfo->addChild(std::move(em));
    hdr->addChild(std::move(hinfo));
    root->addChild(std::move(hdr));

    // Form
    auto fm = std::make_unique<ui::Form>();
    fm->setFont(font);
    fm->addField("Display Name", std::make_unique<ui::TextInput>("Alice Johnson"));
    fm->addField("Email", std::make_unique<ui::TextInput>("alice@example.com"));
    fm->addField("Bio", std::make_unique<ui::TextInput>("Software developer & UI enthusiast"));

    auto lang = std::make_unique<ui::Select>(); lang->setFont(font); lang->setMinSize(0, 60);
    lang->addItem("English"); lang->addItem("Chinese"); lang->addItem("Japanese");
    lang->setSelectedIndex(0);
    fm->addField("Language", std::move(lang));
    root->addChild(std::move(fm));

    // Preferences
    auto prefs = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    prefs->setSpacing(4);
    auto cb1 = std::make_unique<ui::Checkbox>("Email notifications"); cb1->setChecked(true); cb1->setFont(font);
    prefs->addChild(std::move(cb1));
    auto cb2 = std::make_unique<ui::Checkbox>("Dark mode"); cb2->setChecked(true); cb2->setFont(font);
    prefs->addChild(std::move(cb2));
    auto cb3 = std::make_unique<ui::Checkbox>("Public profile"); cb3->setFont(font);
    prefs->addChild(std::move(cb3));
    root->addChild(std::move(prefs));

    // Save button
    auto save = std::make_unique<ui::Button>("Save Changes");
    save->setFont(font); save->setCornerRadius(8); save->setMinSize(0, 38);
    save->setGradient({0.2f,0.6f,0.3f,1},{0.15f,0.45f,0.2f,1});
    save->setHoverColor({0.3f,0.7f,0.4f,1});
    save->setPressColor({0.15f,0.5f,0.25f,1});
    save->setOnClick([](){ ui::showToast("Profile saved!", ui::IconType::Check); printf("[Profile] Saved\n"); });
    root->addChild(std::move(save));

    app.setRoot(std::move(root));
    app.run();
    return 0;
}

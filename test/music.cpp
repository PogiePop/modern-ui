// ModernUI — Music Player Demo
#include <cstdio>
#include <memory>
#include "core/App.hpp"
#include "core/Types.hpp"
#include "widgets/Container.hpp"
#include "widgets/Button.hpp"
#include "widgets/Label.hpp"
#include "widgets/Slider.hpp"
#include "widgets/Avatar.hpp"
#include "widgets/Select.hpp"

using namespace ui;

int main() {
    ui::App app;
    if (!app.init("Music Player - ModernUI", 380, 540)) return -1;
    auto* font = app.getFont();

    auto root = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    root->setPadding(ui::EdgeInsets::all(20));
    root->setSpacing(12);
    root->setBackgroundColor(ui::Color::fromHex(0xFF12121A));

    // Now Playing header
    auto np = std::make_unique<ui::Label>("NOW PLAYING");
    np->setFontSize(11); np->setFont(font);
    np->setColorRole(ColorRole::TextDisabled);
    root->addChild(std::move(np));

    // Album art placeholder
    auto art = std::make_unique<ui::Container>();
    art->setMinSize(0, 200);
    art->setBackgroundColor(ui::Color::fromHex(0xFF2A2A3A));
    root->addChild(std::move(art));

    // Track info
    auto title = std::make_unique<ui::Label>("Midnight Dreams");
    title->setFontSize(18); title->setFont(font);
    title->setColorRole(ColorRole::Text);
    root->addChild(std::move(title));

    auto artist = std::make_unique<ui::Label>("Luna Eclipse");
    artist->setFontSize(13); artist->setFont(font);
    artist->setColorRole(ColorRole::TextSecondary);
    root->addChild(std::move(artist));

    // Progress
    auto prog = std::make_unique<ui::Slider>(0, 217, 73);
    prog->setMinSize(0, 24);
    root->addChild(std::move(prog));

    auto times = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    times->setMinSize(0, 20);
    auto t1 = std::make_unique<ui::Label>("1:13");
    t1->setFontSize(11); t1->setFont(font); t1->setColorRole(ColorRole::TextDisabled);
    times->addChild(std::move(t1));
    auto t2 = std::make_unique<ui::Label>("3:37");
    t2->setFontSize(11); t2->setFont(font); t2->setColorRole(ColorRole::TextDisabled);
    root->addChild(std::move(t2));

    // Controls
    auto ctrls = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    ctrls->setSpacing(12); ctrls->setMinSize(0, 42);
    ctrls->setAlign(ui::Container::Align::Center);

    auto prev = std::make_unique<ui::Button>("|<");
    prev->setFont(font); prev->setCornerRadius(20); prev->setMinSize(40, 40);
    ctrls->addChild(std::move(prev));

    auto play = std::make_unique<ui::Button>(">");
    play->setFont(font); play->setCornerRadius(24); play->setMinSize(48, 48);
    play->setGradient({0.2f,0.5f,0.9f,1},{0.15f,0.35f,0.7f,1});
    play->setHoverColor({0.3f,0.6f,1,1});
    play->setShadow({0,2}, 8, {0,0,0,0.4f});
    ctrls->addChild(std::move(play));

    auto next = std::make_unique<ui::Button>(">|");
    next->setFont(font); next->setCornerRadius(20); next->setMinSize(40, 40);
    ctrls->addChild(std::move(next));
    root->addChild(std::move(ctrls));

    // Volume
    auto volLabel = std::make_unique<ui::Label>("Volume");
    volLabel->setFontSize(12); volLabel->setFont(font);
    volLabel->setColorRole(ColorRole::TextDisabled);
    root->addChild(std::move(volLabel));

    auto vol = std::make_unique<ui::Slider>(0, 100, 75);
    vol->setMinSize(0, 22);
    root->addChild(std::move(vol));

    // Playlist
    auto pl = std::make_unique<ui::Select>(); pl->setFont(font); pl->setMinSize(0, 120);
    pl->addItem("1. Midnight Dreams - Luna Eclipse");
    pl->addItem("2. Starlight - Nova Wave");
    pl->addItem("3. Ocean Breeze - Coastal");
    pl->addItem("4. Neon Nights - Synth City");
    pl->addItem("5. Rainy Day - Jazz Collective");
    pl->setSelectedIndex(0);
    root->addChild(std::move(pl));

    app.setRoot(std::move(root));
    printf("Music Player demo. Use slider for progress and volume.\n");
    app.run();
    return 0;
}

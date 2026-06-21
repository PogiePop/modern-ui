// ModernUI — Login Demo
#include <cstdio>
#include <memory>
#include "core/App.hpp"
#include "core/Types.hpp"
#include "widgets/Container.hpp"
#include "widgets/Button.hpp"
#include "widgets/Label.hpp"
#include "widgets/TextInput.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/Toast.hpp"

using namespace ui;

int main() {
    ui::App app;
    if (!app.init("Login - ModernUI", 380, 420)) return -1;
    auto* font = app.getFont();

    auto root = std::make_unique<ui::Container>(ui::LayoutDirection::Vertical);
    root->setPadding(ui::EdgeInsets::all(32));
    root->setSpacing(14);
    root->setBackgroundColor(ui::Color::fromHex(0xFF1A1A2E));

    // Logo / Title
    auto title = std::make_unique<ui::Label>("Welcome Back");
    title->setFontSize(22); title->setFont(font);
    title->setColorRole(ColorRole::Text);
    root->addChild(std::move(title));

    auto sub = std::make_unique<ui::Label>("Sign in to continue");
    sub->setFontSize(13); sub->setFont(font);
    sub->setColorRole(ColorRole::TextSecondary);
    root->addChild(std::move(sub));

    auto sp = std::make_unique<ui::Container>(); sp->setMinSize(0, 12);
    root->addChild(std::move(sp));

    // Username
    auto ul = std::make_unique<ui::Label>("Username");
    ul->setFontSize(12); ul->setFont(font);
    ul->setColorRole(ColorRole::TextSecondary);
    root->addChild(std::move(ul));

    auto user = std::make_unique<ui::TextInput>("Enter username");
    user->setFont(font); user->setMinSize(0, 34);
    root->addChild(std::move(user));

    // Password
    auto pl = std::make_unique<ui::Label>("Password");
    pl->setFontSize(12); pl->setFont(font);
    pl->setColorRole(ColorRole::TextSecondary);
    root->addChild(std::move(pl));

    auto pass = std::make_unique<ui::TextInput>("Enter password");
    pass->setFont(font); pass->setMinSize(0, 34);
    root->addChild(std::move(pass));

    // Remember + Forgot
    auto row = std::make_unique<ui::Container>(ui::LayoutDirection::Horizontal);
    row->setMinSize(0, 30);
    auto rem = std::make_unique<ui::Checkbox>("Remember me");
    rem->setFont(font); row->addChild(std::move(rem));
    root->addChild(std::move(row));

    // Login button
    auto btn = std::make_unique<ui::Button>("Sign In");
    btn->setFont(font); btn->setCornerRadius(8); btn->setMinSize(0, 40);
    btn->setGradient({0.2f,0.5f,0.9f,1},{0.15f,0.35f,0.7f,1});
    btn->setHoverColor({0.3f,0.6f,1,1}); btn->setPressColor({0.15f,0.4f,0.85f,1});
    btn->setShadow({0,2}, 6, {0,0,0,0.3f});
    btn->setOnClick([](){ ui::showToast("Login successful!", ui::IconType::Check); printf("[Login] Signed in\n"); });
    root->addChild(std::move(btn));

    // Register link
    auto reg = std::make_unique<ui::Label>("Don't have an account? Register");
    reg->setFontSize(12); reg->setFont(font);
    reg->setColorRole(ColorRole::Primary);
    root->addChild(std::move(reg));

    app.setRoot(std::move(root));
    app.run();
    return 0;
}

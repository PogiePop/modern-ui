// ModernUI - CJK Font Rendering Test (MSVC-compatible via \u escapes)
// Build: cmake --build . --target test_cjk --config Release
// Run:   ./Release/test_cjk.exe
#include <cstdio>
#include <memory>
#include <string>

#include "core/App.hpp"
#include "core/Types.hpp"
#include "widgets/Container.hpp"
#include "widgets/Label.hpp"
#include "widgets/ScrollArea.hpp"
#include "widgets/TabView.hpp"
#include "widgets/Space.hpp"
#include "res/Theme.hpp"
#include "res/Font.hpp"

using namespace ui;

static Font* g_font = nullptr;
static Theme g_theme;

static std::unique_ptr<Label> makeLabel(const std::string& text, float size, ColorRole role) {
    auto l = std::make_unique<Label>(text);
    l->setFont(g_font); l->setFontSize(size);
    l->setColorRole(role);
    return l;
}

int main() {
    g_theme = Theme::defaultDark();

    App app;
    if (!app.init("ModernUI - CJK Font Test", 800, 600)) {
        fprintf(stderr, "Failed to init app\n");
        return -1;
    }
    g_font = app.getFont();
    if (!g_font) {
        fprintf(stderr, "No font loaded\n");
        return -1;
    }

    printf("Font lineHeight: %.1f, ascent: %.1f\n", g_font->lineHeight(), g_font->ascent());

    auto root = std::make_unique<Container>(LayoutDirection::Vertical);
    root->setPadding(EdgeInsets::all(20));
    root->setSpacing(10);
    root->setBackgroundColor(g_theme.color(ColorRole::Bg));

    // Title
    auto title = std::make_unique<Label>("CJK Font Rendering Test");
    title->setFont(g_font); title->setFontSize(22);
    title->setColorRole(ColorRole::Primary);
    root->addChild(std::move(title));

    auto note = std::make_unique<Label>("This test verifies dynamic glyph loading for Chinese/Japanese/Korean characters.");
    note->setFont(g_font); note->setFontSize(12);
    note->setColorRole(ColorRole::TextDisabled);
    root->addChild(std::move(note));

    root->addChild(std::make_unique<Divider>());

    // CJK text at various sizes - using \u escapes for MSVC compatibility
    root->addChild(makeLabel("Title 22px", 22, ColorRole::Text));

    root->addChild(makeLabel("中文测试 - Chinese Test", 22, ColorRole::Text));
    root->addChild(makeLabel("日本語 - Japanese Test", 22, ColorRole::Text));
    root->addChild(makeLabel("한국어 - Korean Test", 22, ColorRole::Text));

    root->addChild(std::make_unique<Divider>());

    root->addChild(makeLabel("Body 16px", 16, ColorRole::Text));
    root->addChild(makeLabel("欢迎使用 ModernUI 框架", 16, ColorRole::Text));
    root->addChild(makeLabel("跨平台 C++20 OpenGL 渲染引擎", 16, ColorRole::TextSecondary));

    root->addChild(std::make_unique<Divider>());

    root->addChild(makeLabel("Caption 11px", 11, ColorRole::TextDisabled));
    root->addChild(makeLabel("支持 Unicode 全集字符动态加载", 11, ColorRole::TextDisabled));

    root->addChild(std::make_unique<Divider>());

    // Mixed CJK + ASCII
    root->addChild(makeLabel("Mixed 18px", 18, ColorRole::Text));
    root->addChild(makeLabel("CJK 中文日本語한국어 mixed with ASCII text.", 18, ColorRole::Text));
    root->addChild(makeLabel("12345 一二三四五 abcde あいうえお", 18, ColorRole::Accent));

    root->addChild(std::make_unique<Divider>());

    // Text measurement verification
    root->addChild(makeLabel("Measurement Tests", 14, ColorRole::TextSecondary));
    {
        std::string s1 = "Hello World";
        std::string s2 = "你好世界"; // 你好世界
        float w1 = g_font->measureText(s1);
        float w2 = g_font->measureText(s2);
        char buf[256];
        snprintf(buf, sizeof(buf), "ASCII \"Hello World\" width: %.1f  |  CJK \"Hello World\" width: %.1f", w1, w2);
        auto m = std::make_unique<Label>(buf);
        m->setFont(g_font); m->setFontSize(12);
        m->setColorRole(ColorRole::TextDisabled);
        root->addChild(std::move(m));
    }

    // Apply theme
    root->setTheme(&g_theme);

    app.setRoot(std::move(root));

    printf("\n=== CJK Font Test ===\n");
    printf("Font loaded. Checking glyph cache...\n");
    printf("Manually triggering glyph loads for common CJK chars...\n");

    // Pre-load some common CJK glyphs for faster first paint
    const char* preload[] = {
        "中", "文", "测", "试", "你", "好",
        "日", "本", "語", "欢", "迎",
    };
    for (auto* s : preload) {
        g_font->measureText(s);
    }

    printf("Glyph cache primed. Running app...\n\n");
    app.run();
    return 0;
}

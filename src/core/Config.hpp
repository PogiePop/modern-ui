#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace ui {

// Centralized global configuration — all tunable settings in one place
class Config {
public:
    static Config& instance();

    // ── Rendering ──
    bool multiThreaded = false;           // enable parallel layout/measure
    int renderThreads = 0;                // 0 = auto (hardware concurrency)
    bool vsync = true;                    // GLFW swap interval
    int targetFPS = 60;                   // 0 = unlimited
    bool dirtyRectRendering = false;      // only repaint changed regions

    // ── Theme ──
    bool darkMode = true;                 // initial theme mode
    float themeTransitionDuration = 0.4f; // seconds for theme switch animation

    // ── Font ──
    std::vector<std::string> fontPaths = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
    };
    float defaultFontSize = 16.0f;
    int atlasSize = 1024;                 // glyph atlas texture size

    // ── Input ──
    float doubleClickTime = 0.4f;         // seconds
    float cursorBlinkRate = 0.53f;        // seconds per blink cycle
    float scrollStep = 40.0f;             // pixels per scroll wheel click
    int tooltipDelay = 45;                // frames before tooltip appears

    // ── Animation ──
    float defaultAnimDuration = 0.3f;     // seconds
    bool animationsEnabled = true;

    // ── Debug ──
    bool showFPS = false;
    bool showLayoutBounds = false;
    bool logFrameTiming = false;

    // Apply — sync with running subsystems
    void apply();

    // Load/save to JSON-like file (simple key=value format)
    bool loadFromFile(const char* path);
    bool saveToFile(const char* path) const;

private:
    Config() = default;
};

} // namespace ui

#pragma once
#include <string>
#include <vector>
#include <functional>
#include "core/Icon.hpp"

namespace ui {

class Painter;
class Font;
class Widget;

struct ToastItem {
    std::string message;
    IconType icon = IconType::Info;
    float timer = 0;
    float duration = 3.0f; // seconds
    float offsetY = 0;     // animation offset
};

class ToastManager {
public:
    static ToastManager& instance();

    void show(const std::string& msg, IconType icon = IconType::Info, float duration = 3.0f);
    void update(float dt);
    void render(Painter& painter, Font* font, float screenW);
    bool hasActive() const;

private:
    std::vector<ToastItem> m_toasts;
};

// Convenience
void showToast(const std::string& msg, IconType icon = IconType::Info, float duration = 3.0f);

} // namespace ui

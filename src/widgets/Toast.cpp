#include "Toast.hpp"
#include "core/Painter.hpp"
#include "core/Icon.hpp"
#include "res/Font.hpp"
#include <algorithm>

namespace ui {

ToastManager& ToastManager::instance() { static ToastManager mgr; return mgr; }

void ToastManager::show(const std::string& msg, IconType icon, float duration) {
    ToastItem t; t.message = msg; t.icon = icon; t.duration = duration; t.timer = 0; t.offsetY = -60;
    m_toasts.push_back(t);
}

void ToastManager::update(float dt) {
    for (auto& t : m_toasts) {
        t.timer += dt;
        // Slide in
        if (t.timer < 0.3f) t.offsetY = -60 + (t.timer / 0.3f) * 60;
        // Slide out
        else if (t.timer > t.duration - 0.3f) {
            float outT = (t.timer - (t.duration - 0.3f)) / 0.3f;
            t.offsetY = outT * 60;
        }
        else t.offsetY = 0;
    }
    // Remove expired
    m_toasts.erase(std::remove_if(m_toasts.begin(), m_toasts.end(),
        [](const ToastItem& t) { return t.timer > t.duration; }), m_toasts.end());
}

bool ToastManager::hasActive() const { return !m_toasts.empty(); }

void ToastManager::render(Painter& painter, Font* font, float screenW) {
    float lh = font ? font->lineHeight() : 16;
    float y = 20;
    for (auto& t : m_toasts) {
        float tw = font ? font->measureText(t.message) : t.message.size()*8.0f;
        float bw = tw + 60, bh = lh + 20;
        if (bw > screenW - 40) bw = screenW - 40;
        float bx = (screenW - bw) * 0.5f;
        Color bgCol = t.icon == IconType::Warning ? Color::fromHex(0xDD332211) :
                       t.icon == IconType::Error   ? Color::fromHex(0xDD331111) :
                       Color::fromHex(0xDD112233);
        painter.drawRoundedRect({bx, y + t.offsetY, bw, bh}, bgCol, 8);
        if (t.icon != IconType::None)
            drawIcon(painter, t.icon, bx + 8, y + t.offsetY + 8, lh, Color{1,1,1,1});
        float tx = (t.icon != IconType::None) ? bx + lh + 20 : bx + 12;
        painter.drawText({tx, y + t.offsetY + 10, bw - (tx-bx) - 12, lh}, t.message,
                         Color{1,1,1,1}, TextAlign::Left);
        y += bh + 8;
    }
}

void showToast(const std::string& msg, IconType icon, float duration) {
    ToastManager::instance().show(msg, icon, duration);
}

} // namespace ui

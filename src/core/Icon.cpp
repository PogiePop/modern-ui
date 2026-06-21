#include "Icon.hpp"
#include "Painter.hpp"
#include <cmath>

namespace ui {

void drawIcon(Painter& p, IconType icon, float x, float y, float size, const Color& color) {
    if (icon == IconType::None || size < 8) return;
    float cx = x + size * 0.5f;
    float cy = y + size * 0.5f;
    float s = size * 0.12f;
    if (s < 1.2f) s = 1.2f;

    auto hr = [&](float rx, float ry, float rw, float rh) { p.drawRect({rx, ry, rw, rh}, color); };
    auto rr = [&](float rx, float ry, float rw, float rh, float rad) { p.drawRoundedRect({rx, ry, rw, rh}, color, rad); };
    auto circle = [&](float ccx, float ccy, float r) { p.drawRoundedRect({ccx - r, ccy - r, r * 2, r * 2}, color, r); };

    switch (icon) {
    case IconType::ArrowUp:
        hr(cx - s * 0.3f, cy - s * 0.2f, s * 0.6f, s * 2);
        hr(cx - s * 1.2f, cy - s * 0.8f, s * 2.4f, s);
        hr(cx - s * 0.8f, cy + s * 0.2f, s * 1.6f, s);
        break;
    case IconType::ArrowDown:
        hr(cx - s * 0.3f, cy - s * 1.8f, s * 0.6f, s * 2);
        hr(cx - s * 1.2f, cy + s * 0.8f - s, s * 2.4f, s);
        hr(cx - s * 0.8f, cy - s * 0.2f - s, s * 1.6f, s);
        break;
    case IconType::ArrowLeft:
        hr(cx - s * 1.8f, cy - s * 0.3f, s * 2, s * 0.6f);
        hr(cx - s * 0.8f, cy - s * 1.2f, s, s * 2.4f);
        hr(cx + s * 0.2f, cy - s * 0.8f, s, s * 1.6f);
        break;
    case IconType::ArrowRight:
        hr(cx - s * 0.2f, cy - s * 0.3f, s * 2, s * 0.6f);
        hr(cx + s * 0.8f - s, cy - s * 1.2f, s, s * 2.4f);
        hr(cx - s * 0.2f - s, cy - s * 0.8f, s, s * 1.6f);
        break;
    case IconType::ChevronUp:
        hr(cx - s * 1.2f, cy + s * 0.5f, s * 2.4f, s);
        hr(cx - s * 0.7f, cy - s * 0.3f, s * 1.4f, s);
        hr(cx - s * 0.3f, cy - s * 1.2f, s * 0.6f, s);
        break;
    case IconType::ChevronDown:
        hr(cx - s * 1.2f, cy - s * 0.5f - s, s * 2.4f, s);
        hr(cx - s * 0.7f, cy + s * 0.3f - s, s * 1.4f, s);
        hr(cx - s * 0.3f, cy + s * 1.2f - s, s * 0.6f, s);
        break;
    case IconType::Check:
        hr(cx - s * 1.2f, cy - s * 0.2f, s * 0.8f, s * 0.7f);
        hr(cx - s * 0.4f, cy + s * 0.3f, s * 1.6f, s * 0.7f);
        break;
    case IconType::Close:
        hr(cx - s * 1.5f, cy - s * 0.3f, s * 3, s * 0.6f);
        hr(cx - s * 0.3f, cy - s * 1.5f, s * 0.6f, s * 3);
        break;
    case IconType::Plus:
        hr(cx - s * 0.3f, cy - s * 1.8f, s * 0.6f, s * 3.6f);
        hr(cx - s * 1.8f, cy - s * 0.3f, s * 3.6f, s * 0.6f);
        break;
    case IconType::Minus:
        hr(cx - s * 1.8f, cy - s * 0.3f, s * 3.6f, s * 0.6f);
        break;
    case IconType::Info:
        circle(cx, cy, s * 2.5f);
        circle(cx, cy, s * 1.8f);
        hr(cx - s * 0.2f, cy - s * 1.2f, s * 0.4f, s * 1.8f);
        hr(cx - s * 0.2f, cy + s * 0.8f, s * 0.4f, s * 0.6f);
        break;
    case IconType::Warning:
        hr(cx - s * 1.8f, cy + s * 1, s * 3.6f, s);
        hr(cx - s * 1.5f, cy + s * 0.2f, s * 3, s);
        hr(cx - s * 1, cy - s * 0.5f, s * 2, s);
        hr(cx - s * 0.4f, cy - s * 1.5f, s * 0.8f, s);
        hr(cx - s * 0.2f, cy - s * 0.2f, s * 0.4f, s * 1.5f);
        hr(cx - s * 0.2f, cy + s * 1.3f, s * 0.4f, s * 0.6f);
        break;
    case IconType::Error:
        circle(cx, cy, s * 2.5f);
        hr(cx - s * 1.5f, cy - s * 0.3f, s * 3, s * 0.6f);
        hr(cx - s * 0.3f, cy - s * 1.5f, s * 0.6f, s * 3);
        break;
    case IconType::Question:
        circle(cx, cy, s * 2.5f);
        circle(cx, cy, s * 1.8f);
        hr(cx - s * 1, cy - s * 1, s * 0.6f, s * 0.5f);
        hr(cx + s * 0.4f - s * 0.3f, cy - s * 1, s * 0.6f, s * 1.2f);
        hr(cx - s * 0.8f, cy - s * 0.2f, s * 1.6f, s * 0.5f);
        hr(cx - s * 0.2f, cy + s * 0.8f, s * 0.4f, s * 0.6f);
        break;
    case IconType::Search:
        circle(cx - s * 0.6f, cy - s * 0.6f, s * 2);
        circle(cx - s * 0.6f, cy - s * 0.6f, s * 1.3f);
        hr(cx + s * 0.8f, cy + s * 0.8f, s * 1.2f, s * 0.5f);
        break;
    case IconType::User:
        circle(cx, cy - s * 1, s * 1.8f);
        circle(cx, cy - s * 1, s * 1.1f);
        rr(cx - s * 2.5f, cy + s * 0.5f, s * 5, s * 3.5f, s * 3.5f * 0.5f);
        break;
    case IconType::Settings:
        circle(cx, cy, s * 1.5f);
        circle(cx, cy, s * 1.5f - s * 0.5f);
        for (int i = 0; i < 6; ++i) {
            float a = i * 1.0472f;
            float tx = cx + cosf(a) * s * 2.5f;
            float ty = cy + sinf(a) * s * 2.5f;
            circle(tx, ty, s * 0.8f);
        }
        break;
    case IconType::Home:
        hr(cx - s * 2.5f, cy + s * 0.5f, s * 5, s * 3);
        hr(cx - s * 1.8f, cy - s * 1, s * 3.6f, s);
        hr(cx - s * 1.2f, cy - s * 1.8f, s * 2.4f, s);
        hr(cx - s * 0.8f, cy + s * 1.5f, s * 1.6f, s * 2);
        break;
    case IconType::Heart:
        circle(cx - s * 2, cy - s * 1.2f, s * 1.8f);
        circle(cx + s * 2, cy - s * 1.2f, s * 1.8f);
        hr(cx - s * 2.8f, cy - s * 0.2f, s * 5.6f, s * 3.5f);
        hr(cx - s * 0.2f, cy - s * 0.2f, s * 0.4f, s * 3.2f);
        break;
    case IconType::Star:
        hr(cx - s * 0.3f, cy - s * 2.5f, s * 0.6f, s * 5);
        hr(cx - s * 2.5f, cy - s * 0.3f, s * 5, s * 0.6f);
        hr(cx - s * 1.5f, cy - s * 1.5f, s * 3, s * 0.5f);
        hr(cx - s * 1.5f, cy + s * 1, s * 3, s * 0.5f);
        break;
    case IconType::Mail:
        hr(cx - s * 2.5f, cy - s * 1.5f, s * 5, s * 3);
        hr(cx - s * 2, cy - s * 1.5f, s * 0.5f, s * 2);
        hr(cx + s * 1.5f, cy - s * 1.5f, s * 0.5f, s * 2);
        hr(cx - s * 2, cy, s * 4, s * 0.5f);
        hr(cx, cy, s * 0.5f, s * 1.5f);
        break;
    case IconType::Lock:
        rr(cx - s * 2, cy - s * 0.5f, s * 4, s * 3.5f, s * 0.8f);
        rr(cx - s * 1.5f, cy - s * 2.5f, s * 3, s * 2.5f, s * 2);
        hr(cx - s * 0.3f, cy, s * 0.6f, s * 1.5f);
        circle(cx, cy + s * 0.2f, s * 0.8f);
        break;
    case IconType::Play:
        hr(cx - s * 1.2f, cy - s * 2, s * 0.5f, s * 4);
        hr(cx + s * 1.2f - s * 0.5f, cy - s * 2, s * 0.5f, s * 4);
        hr(cx - s * 1.2f, cy - s * 2, s * 2.5f, s * 0.5f);
        hr(cx - s * 1.2f, cy + s * 1.5f, s * 2.5f, s * 0.5f);
        break;
    case IconType::Pause:
        hr(cx - s * 1.8f, cy - s * 2, s * 1.2f, s * 4);
        hr(cx + s * 0.6f, cy - s * 2, s * 1.2f, s * 4);
        break;
    case IconType::Stop:
        rr(cx - s * 2.2f, cy - s * 2.2f, s * 4.4f, s * 4.4f, s * 0.6f);
        break;
    case IconType::Refresh:
        circle(cx, cy, s * 2.5f);
        circle(cx, cy, s * 1.8f);
        hr(cx + s * 1.5f, cy - s * 2, s * 0.5f, s * 2);
        hr(cx - s * 0.5f, cy - s * 2.5f, s * 2, s * 0.5f);
        break;
    case IconType::File:
        hr(cx - s * 2, cy - s * 2.5f, s * 3.5f, s * 5);
        hr(cx + s * 0.5f, cy - s * 2.5f, s * 1, s * 1.5f);
        hr(cx - s * 2, cy - s * 1, s * 2.5f, s * 0.4f);
        hr(cx - s * 2, cy + s * 0.2f, s * 1.8f, s * 0.4f);
        hr(cx - s * 2, cy + s * 1.4f, s * 2.2f, s * 0.4f);
        break;
    case IconType::Folder:
        hr(cx - s * 2.5f, cy - s * 0.5f, s * 5, s * 3.5f);
        rr(cx - s * 2.5f, cy - s * 2, s * 2.5f, s * 1.8f, s * 0.8f);
        break;
    case IconType::Download:
        hr(cx - s * 0.3f, cy - s * 2.5f, s * 0.6f, s * 3.5f);
        hr(cx - s * 1.8f, cy + s * 1, s * 3.6f, s * 0.5f);
        hr(cx - s * 1.2f, cy + s * 0.3f, s * 2.4f, s * 0.5f);
        hr(cx - s * 2.5f, cy + s * 2, s * 5, s * 0.5f);
        break;
    case IconType::Upload:
        hr(cx - s * 0.3f, cy - s * 2.5f, s * 0.6f, s * 3.5f);
        hr(cx - s * 1.8f, cy - s * 1.5f, s * 3.6f, s * 0.5f);
        hr(cx - s * 1.2f, cy - s * 1, s * 2.4f, s * 0.5f);
        hr(cx - s * 2.5f, cy + s * 2, s * 5, s * 0.5f);
        break;
    case IconType::Image:
        hr(cx - s * 2.5f, cy - s * 2, s * 5, s * 4);
        circle(cx - s * 1, cy - s * 0.5f, s * 1.5f);
        hr(cx - s * 2, cy + s * 0.5f, s * 4, s * 1.5f);
        break;
    case IconType::Filter:
        hr(cx - s * 2.5f, cy - s * 0.3f, s * 5, s * 0.6f);
        hr(cx - s * 1.5f, cy + s * 0.5f, s * 3, s * 0.6f);
        hr(cx - s * 0.8f, cy + s * 1.5f, s * 1.6f, s * 0.6f);
        break;
    case IconType::Grid:
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 2; ++c)
                rr(cx - s * 1.5f + c * s * 1.6f, cy - s * 1.5f + r * s * 1.6f, s * 1.4f, s * 1.4f, s * 0.3f);
        break;
    case IconType::List:
        for (int i = 0; i < 3; ++i) {
            hr(cx - s * 2.5f, cy - s * 1 + i * s * 1.5f, s * 5, s * 0.5f);
            circle(cx - s * 2, cy - s * 0.8f + i * s * 1.5f, s * 0.5f);
        }
        break;
    case IconType::Copy:
        hr(cx - s * 0.5f, cy - s * 2, s * 3, s * 3.5f);
        hr(cx - s * 2, cy - s * 0.5f, s * 3, s * 3.5f);
        break;
    case IconType::Edit:
        hr(cx - s * 1.5f, cy + s * 1, s * 4, s * 0.5f);
        hr(cx - s * 0.2f, cy - s * 2, s * 0.5f, s * 3.5f);
        break;
    case IconType::Volume:
        hr(cx - s * 1.5f, cy - s * 1, s * 1.5f, s * 2);
        hr(cx - s * 0.5f, cy - s * 1.5f, s * 0.5f, s * 3);
        hr(cx + s * 0.5f, cy - s * 0.3f, s * 1, s * 0.6f);
        hr(cx + s * 1.2f, cy - s * 0.8f, s * 0.6f, s * 1.6f);
        break;
    case IconType::Mute:
        hr(cx - s * 1.5f, cy - s * 1, s * 1.5f, s * 2);
        hr(cx - s * 0.5f, cy - s * 1.5f, s * 0.5f, s * 3);
        hr(cx + s * 0.8f, cy - s * 1.2f, s * 0.5f, s * 2.4f);
        hr(cx + s * 0.2f, cy - s * 1.2f, s * 1.5f, s * 0.5f);
        break;
    case IconType::Clock:
        circle(cx, cy, s * 2.8f);
        circle(cx, cy, s * 2.2f);
        hr(cx - s * 0.15f, cy - s * 2, s * 0.3f, s * 1.5f);
        hr(cx - s * 0.15f, cy - s * 1.5f, s * 0.3f, s * 2.5f);
        break;
    case IconType::Calendar:
        hr(cx - s * 2, cy - s * 2, s * 4, s * 1.2f);
        hr(cx - s * 2, cy - s * 0.5f, s * 4, s * 3);
        hr(cx - s * 1.2f, cy - s * 2.5f, s * 0.5f, s * 2);
        hr(cx + s * 0.8f, cy - s * 2.5f, s * 0.5f, s * 2);
        break;
    case IconType::CheckCircle:
        circle(cx, cy, s * 2.8f);
        circle(cx, cy, s * 2.2f);
        hr(cx - s * 1.2f, cy - s * 0.2f, s * 0.8f, s * 0.6f);
        hr(cx - s * 0.3f, cy + s * 0.2f, s * 1.5f, s * 0.6f);
        break;
    case IconType::CloseCircle:
        circle(cx, cy, s * 2.8f);
        circle(cx, cy, s * 2.2f);
        hr(cx - s * 1.2f, cy - s * 0.2f, s * 2.4f, s * 0.4f);
        hr(cx - s * 0.2f, cy - s * 1.2f, s * 0.4f, s * 2.4f);
        break;
    case IconType::Menu:
        for (int i = 0; i < 3; ++i)
            hr(cx - s * 2, cy - s * 1.2f + i * s * 1.2f, s * 4, s * 0.45f);
        break;
    case IconType::More:
        for (int i = 0; i < 3; ++i)
            circle(cx, cy - s * 1.5f + i * s * 1.5f, s * 0.6f);
        break;
    case IconType::External:
        hr(cx - s * 1.5f, cy - s * 1, s * 2.5f, s * 2);
        hr(cx - s * 1.2f, cy - s * 1.5f, s * 0.4f, s * 2);
        hr(cx - s * 1.5f, cy - s * 2, s * 1.5f, s * 0.4f);
        break;
    case IconType::Eye:
        circle(cx, cy, s * 2);
        circle(cx, cy, s * 1.2f);
        circle(cx, cy, s * 0.7f);
        break;
    case IconType::EyeOff:
        circle(cx, cy, s * 2);
        hr(cx - s * 1.8f, cy - s * 0.15f, s * 3.6f, s * 0.3f);
        break;
    case IconType::Bell:
        rr(cx - s * 1.5f, cy - s * 1.5f, s * 3, s * 3.5f, s * 2);
        hr(cx - s * 0.3f, cy + s * 2, s * 0.6f, s * 0.8f);
        circle(cx, cy - s * 1.8f, s * 0.8f);
        break;
    case IconType::Tag:
        hr(cx - s * 1, cy - s * 1.5f, s * 3, s * 3);
        circle(cx - s * 0.5f, cy, s * 0.8f);
        hr(cx + s * 1.5f, cy - s * 1.2f, s * 1, s * 2.4f);
        break;
    case IconType::Success:
        circle(cx, cy, s * 2.8f);
        hr(cx - s * 1.2f, cy - s * 0.2f, s * 0.8f, s * 0.6f);
        hr(cx - s * 0.3f, cy + s * 0.2f, s * 1.5f, s * 0.6f);
        break;
    default:
        circle(cx, cy, s);
        break;
    }
}

} // namespace ui

#include "Icon.hpp"
#include "Painter.hpp"

namespace ui {

void drawIcon(Painter& p, IconType icon, float x, float y, float size, const Color& color) {
    if (icon == IconType::None) return;
    float cx = x + size * 0.5f, cy = y + size * 0.5f;
    float r = size * 0.45f;
    float thick = size * 0.08f; if (thick < 1.5f) thick = 1.5f;

    auto rect = [&](float rx, float ry, float rw, float rh) { p.drawRect({rx, ry, rw, rh}, color); };
    auto circ = [&](float crx, float cry, float crr) { // approximate circle with rect for now
        p.drawRoundedRect({crx-crr, cry-crr, crr*2, crr*2}, color, crr);
    };

    switch (icon) {
    case IconType::Info:
        circ(cx, cy, r); circ(cx, cy, r-thick); // ring
        rect(cx-thick*0.5f, cy-r*0.55f, thick, r*0.55f); // i body
        rect(cx-thick*0.5f, cy+r*0.15f, thick, thick*1.5f); // dot
        break;
    case IconType::Warning: {
        float h = r*1.5f, w = r*1.7f;
        p.drawRect({cx-w*0.5f, cy-h*0.4f, w, thick}, color); p.drawRect({cx-w*0.5f, cy-h*0.4f, thick, h}, color);
        p.drawRect({cx+w*0.5f-thick, cy-h*0.4f, thick, h}, color); p.drawRect({cx-w*0.5f, cy+h*0.4f-thick, w, thick}, color);
        rect(cx-thick*0.5f, cy-thick*2, thick, r*0.6f); // ! body
        rect(cx-thick*0.5f, cy+r*0.15f, thick, thick*1.5f); // ! dot
        break;
    }
    case IconType::Error:
        circ(cx, cy, r); rect(cx-r*0.65f, cy-thick*0.5f, r*1.3f, thick); // X line 1
        rect(cx-thick*0.5f, cy-r*0.65f, thick, r*1.3f); // X line 2 (cross)
        break;
    case IconType::Question:
        circ(cx, cy, r); circ(cx, cy, r-thick);
        rect(cx-r*0.3f, cy-r*0.3f, thick, r*0.3f); rect(cx, cy-r*0.3f, r*0.3f, thick);
        rect(cx+r*0.15f, cy-r*0.3f, thick, r*0.5f); rect(cx-r*0.3f, cy+thick, r*0.6f, thick);
        rect(cx-thick*0.5f, cy+r*0.15f, thick, thick*1.5f); // dot
        break;
    case IconType::Check:
        rect(cx-r*0.25f, cy-thick*0.5f, r*0.5f, thick); // upper segment
        rect(cx-thick*0.3f, cy, r*0.25f, thick); // lower segment
        rect(cx-r*0.22f, cy-thick*0.5f, thick, r*0.7f); // vertical connector
        break;
    case IconType::Close:
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick);
        rect(cx-thick*0.5f, cy-r*0.6f, thick, r*1.2f);
        break;
    case IconType::Plus:
        rect(cx-thick*0.5f, cy-r*0.6f, thick, r*1.2f); // vertical
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick); // horizontal
        break;
    case IconType::Minus:
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick);
        break;
    case IconType::ArrowUp:
        rect(cx-thick*0.5f, cy-r*0.6f, thick, r*1.2f); // stem
        rect(cx-r*0.5f, cy-r*0.3f, r, thick); // head top
        rect(cx-r*0.3f, cy-r*0.1f, r*0.6f, thick); // head mid
        break;
    case IconType::ArrowDown:
        rect(cx-thick*0.5f, cy-r*0.6f, thick, r*1.2f);
        rect(cx-r*0.5f, cy+r*0.3f-thick, r, thick);
        rect(cx-r*0.3f, cy+r*0.1f-thick, r*0.6f, thick);
        break;
    case IconType::ArrowLeft:
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick);
        rect(cx-r*0.3f, cy-r*0.5f, thick, r);
        rect(cx-r*0.1f, cy-r*0.3f, thick, r*0.6f);
        break;
    case IconType::ArrowRight:
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick);
        rect(cx+r*0.3f-thick, cy-r*0.5f, thick, r);
        rect(cx+r*0.1f-thick, cy-r*0.3f, thick, r*0.6f);
        break;
    case IconType::Search:
        circ(cx-r*0.1f, cy-r*0.1f, r*0.7f); circ(cx-r*0.1f, cy-r*0.1f, r*0.7f-thick);
        rect(cx+r*0.25f, cy+r*0.25f, r*0.4f, thick); // handle
        break;
    case IconType::User:
        circ(cx, cy-r*0.25f, r*0.35f); // head
        circ(cx, cy-r*0.25f, r*0.35f-thick);
        rect(cx-r*0.7f, cy+r*0.1f, r*1.4f, r*0.55f); // body - simplified
        break;
    case IconType::Settings:
        circ(cx, cy, r*0.35f); circ(cx, cy, r*0.35f-thick); // center
        for (int i = 0; i < 8; ++i) { // gear teeth
            float a = i * 0.785398f;
            float tx = cx + cosf(a)*r*0.6f, ty = cy + sinf(a)*r*0.6f;
            rect(tx-thick, ty-thick, thick*2, thick*2);
        }
        break;
    case IconType::Home:
        rect(cx-r*0.7f, cy+thick, r*1.4f, r*0.4f); // base
        rect(cx-thick*0.5f, cy-r*0.5f, thick, r*0.5f+thick); // chimney
        rect(cx-r*0.65f, cy-r*0.3f, r*1.3f, thick); // roof
        rect(cx-r*0.45f, cy-r*0.5f, r*0.9f, thick); // roof top
        break;
    case IconType::Heart:
        rect(cx-r*0.55f, cy-r*0.2f, r*1.1f, r*0.7f); // simplified heart
        rect(cx-r*0.3f, cy-r*0.5f, r*0.6f, r*0.3f); // left bump
        rect(cx-thick*0.5f, cy-r*0.3f, r*0.55f, thick); // middle
        break;
    case IconType::Star:
        rect(cx-thick*0.5f, cy-r*0.6f, thick, r*1.2f); // vertical
        rect(cx-r*0.6f, cy-thick*0.5f, r*1.2f, thick); // horizontal
        rect(cx-r*0.4f, cy-r*0.4f, r*0.8f, thick); // diagonal 1
        rect(cx-r*0.4f, cy+r*0.4f-thick, r*0.8f, thick); // diagonal 2
        break;
    case IconType::Mail:
        rect(cx-r*0.65f, cy-r*0.35f, r*1.3f, r*0.7f); // envelope
        rect(cx-r*0.05f, cy-r*0.35f, thick, r*0.35f); // V left
        rect(cx-r*0.05f, cy-r*0.35f, r*0.5f, thick); // V right
        break;
    case IconType::Lock:
        rect(cx-r*0.5f, cy, r, r*0.4f); // body
        rect(cx-r*0.3f, cy-r*0.35f, r*0.6f, r*0.4f); // shackle
        rect(cx-thick*0.5f, cy-r*0.05f, thick, r*0.1f); // keyhole
        break;
    case IconType::Play:
        rect(cx-r*0.3f, cy-r*0.5f, r*0.8f, thick); // top
        rect(cx-r*0.3f, cy+r*0.5f-thick, r*0.8f, thick); // bottom
        rect(cx+r*0.3f-thick, cy-r*0.5f, thick, r); // right edge
        rect(cx-r*0.3f, cy-r*0.5f, thick, r); // left edge
        break;
    case IconType::Pause:
        rect(cx-r*0.4f, cy-r*0.5f, r*0.3f, r); // left bar
        rect(cx+r*0.1f, cy-r*0.5f, r*0.3f, r); // right bar
        break;
    case IconType::Stop:
        rect(cx-r*0.55f, cy-r*0.55f, r*1.1f, r*1.1f);
        break;
    case IconType::File:
        rect(cx-r*0.5f, cy-r*0.55f, r*0.85f, r*1.1f); // body
        rect(cx-r*0.5f, cy-r*0.55f, r*0.85f, thick); // top edge
        rect(cx+r*0.1f, cy-r*0.2f, r*0.5f, thick); // fold
        rect(cx+r*0.2f, cy-r*0.1f, r*0.4f, thick);
        break;
    case IconType::Folder:
        rect(cx-r*0.65f, cy-thick*2, r*1.3f, r*0.75f); // body
        rect(cx-r*0.65f, cy-r*0.3f, r*0.5f, r*0.35f); // tab
        break;
    case IconType::Download:
        rect(cx-thick*0.5f, cy-r*0.5f, thick, r*0.8f); // stem
        rect(cx-r*0.4f, cy+r*0.15f, r*0.8f, thick); // arrow head
        rect(cx-r*0.25f, cy, r*0.5f, thick);
        rect(cx-r*0.55f, cy+r*0.4f, r*1.1f, thick); // base
        break;
    case IconType::Upload:
        rect(cx-thick*0.5f, cy-r*0.5f, thick, r*0.8f);
        rect(cx-r*0.4f, cy-r*0.35f, r*0.8f, thick);
        rect(cx-r*0.25f, cy-r*0.2f, r*0.5f, thick);
        rect(cx-r*0.55f, cy+r*0.4f, r*1.1f, thick);
        break;
    default: break;
    }
}

} // namespace ui

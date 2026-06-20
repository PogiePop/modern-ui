#pragma once

namespace ui {

enum class SizingType {
    Fixed,      // exact pixels
    Fill,       // take remaining space proportionally
    Intrinsic   // size to content
};

struct Sizing {
    SizingType type = SizingType::Intrinsic;
    float value = 0; // for Fixed: pixels; for Fill: flex weight (default 1)

    static Sizing fixed(float px) { return {SizingType::Fixed, px}; }
    static Sizing fill(float flex = 1.0f) { return {SizingType::Fill, flex}; }
    static Sizing intrinsic() { return {SizingType::Intrinsic, 0}; }
};

enum class LayoutDirection {
    Horizontal,
    Vertical
};

} // namespace ui

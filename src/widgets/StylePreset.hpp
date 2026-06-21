#pragma once
#include "core/Types.hpp"
#include "res/Theme.hpp"
#include <cstdio>

namespace ui {

// Pre-built modern style presets applicable to multiple components
namespace StylePreset {

// ── Card styles ──
struct CardStyle { Color bg; Color border; float radius; float elevation; float borderWidth; };
inline CardStyle cardDefault(const Theme& t)   { return {t.color(ColorRole::Surface), t.color(ColorRole::Border), 8, 4, 1}; }
inline CardStyle cardElevated(const Theme& t)  { return {t.color(ColorRole::Surface), t.color(ColorRole::Border), 12, 12, 1}; }
inline CardStyle cardOutlined(const Theme& t)  { return {Color{0,0,0,0}, t.color(ColorRole::Border), 8, 0, 2}; }
inline CardStyle cardFilled(const Theme& t)    { return {t.color(ColorRole::BgSecondary), t.color(ColorRole::Border), 8, 0, 0}; }

// ── Button styles (compose with Variant) ──
struct ButtonStyle { float radius; float padH; float padV; bool shadow; };
inline ButtonStyle btnDefault()     { return {8, 20, 10, false}; }
inline ButtonStyle btnPill()        { return {999, 24, 10, false}; } // fully rounded
inline ButtonStyle btnSoft()        { return {10, 20, 10, true}; }   // with shadow
inline ButtonStyle btnCompact()     { return {6, 14, 6, false}; }    // small padding

// ── Input styles ──
struct InputStyle { float radius; Color bg; Color border; float borderW; };
inline InputStyle inputDefault(const Theme& t)  { return {6, t.color(ColorRole::Surface), t.color(ColorRole::Border), 1}; }
inline InputStyle inputFilled(const Theme& t)   { return {0, t.color(ColorRole::BgSecondary), t.color(ColorRole::Transparent), 0}; }
inline InputStyle inputOutlined(const Theme& t) { return {6, Color{0,0,0,0}, t.color(ColorRole::Border), 2}; }

// ── Switch sizes ──
struct SwitchStyle { float w; float h; };
inline SwitchStyle swDefault() { return {44, 24}; }
inline SwitchStyle swSmall()   { return {32, 18}; }
inline SwitchStyle swLarge()   { return {56, 30}; }

// ── Progress colors ──
inline Color progressPrimary(const Theme& t)  { return t.color(ColorRole::Primary); }
inline Color progressSuccess(const Theme& t)  { return t.color(ColorRole::Success); }
inline Color progressDanger(const Theme& t)   { return t.color(ColorRole::Danger); }
inline Color progressWarning(const Theme& t)  { return t.color(ColorRole::Warning); }
inline Color progressAccent(const Theme& t)   { return t.color(ColorRole::Accent); }

// ── Apply helpers ──
template<typename W>
void applyCard(W* card, const CardStyle& s) {
    card->setBackgroundColor(s.bg);
    card->setCornerRadius ? card->setCornerRadius(s.radius) : (void)0;
    if (s.elevation > 0) card->setShadow({0, s.elevation*0.5f}, s.elevation*1.5f, Color{0,0,0,0.15f});
}

} // namespace StylePreset
} // namespace ui

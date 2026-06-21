#pragma once
#include "core/Types.hpp"

namespace ui {
namespace style {

// Modern default style tokens
inline Color bg()          { return Color::fromHex(0xFF0F0F16); }
inline Color surface()     { return Color::fromHex(0xFF1A1A26); }
inline Color surfaceHigh() { return Color::fromHex(0xFF242438); }
inline Color border()      { return Color::fromHex(0xFF2E2E3E); }
inline Color text()        { return Color::fromHex(0xFFE4E4EC); }
inline Color textDim()     { return Color::fromHex(0xFF8888A0); }
inline Color primary()     { return Color::fromHex(0xFF4F8CF7); }
inline Color primaryHover() { return Color::fromHex(0xFF669DF8); }
inline Color success()     { return Color::fromHex(0xFF34D399); }
inline Color danger()      { return Color::fromHex(0xFFF87171); }
inline Color warning()     { return Color::fromHex(0xFFFBBF24); }

inline EdgeInsets pad()    { return EdgeInsets::all(12); }
inline EdgeInsets padSm()  { return EdgeInsets::all(8); }
inline EdgeInsets padLg()  { return EdgeInsets::all(20); }
inline float gap()         { return 10.0f; }
inline float gapSm()       { return 6.0f; }

} // namespace style
} // namespace ui

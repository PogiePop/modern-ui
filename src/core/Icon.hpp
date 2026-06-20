#pragma once
#include "core/Types.hpp"

namespace ui {

class Painter;

enum class IconType {
    None = 0,
    Info, Warning, Error, Question,
    Check, Close, Plus, Minus,
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
    Search, User, Settings, Home,
    Heart, Star, Mail, Lock,
    Play, Pause, Stop,
    File, Folder, Download, Upload,
    _Count
};

void drawIcon(Painter& p, IconType icon, float x, float y, float size, const Color& color);

} // namespace ui

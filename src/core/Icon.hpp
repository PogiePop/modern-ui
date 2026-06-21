#pragma once
#include "core/Types.hpp"

namespace ui {

class Painter;

enum class IconType {
    None = 0,
    // Status
    Info, Warning, Error, Question, Success,
    // Actions
    Check, Close, Plus, Minus, CheckCircle, CloseCircle,
    // Navigation
    ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
    ChevronUp, ChevronDown,
    // Objects
    Search, User, Settings, Home, Heart, Star, Mail, Lock,
    // Media
    Play, Pause, Stop, Refresh, Volume, Mute, Clock,
    // Files
    File, Folder, Download, Upload, Image, Copy, Edit,
    // UI
    Menu, More, External, Filter, Grid, List,
    Eye, EyeOff, Bell, Calendar, Tag,
    _Count
};

void drawIcon(Painter& p, IconType icon, float x, float y, float size, const Color& color);

} // namespace ui

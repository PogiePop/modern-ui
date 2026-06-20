#pragma once

#include <cstdint>

namespace ui {

struct Point {
    float x = 0;
    float y = 0;

    Point operator+(const Point& other) const { return {x + other.x, y + other.y}; }
    Point operator-(const Point& other) const { return {x - other.x, y - other.y}; }
};

struct Size {
    float width = 0;
    float height = 0;

    bool isEmpty() const { return width <= 0 || height <= 0; }
};

struct Rect {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    static Rect fromXYWH(float x, float y, float w, float h) { return {x, y, w, h}; }

    float right() const { return x + width; }
    float bottom() const { return y + height; }
    Point topLeft() const { return {x, y}; }
    Point center() const { return {x + width * 0.5f, y + height * 0.5f}; }
    Size size() const { return {width, height}; }

    bool isEmpty() const { return width <= 0 || height <= 0; }

    bool contains(const Point& pt) const {
        return pt.x >= x && pt.x < right() && pt.y >= y && pt.y < bottom();
    }

    Rect intersect(const Rect& other) const {
        float nx = x > other.x ? x : other.x;
        float ny = y > other.y ? y : other.y;
        float nr = right() < other.right() ? right() : other.right();
        float nb = bottom() < other.bottom() ? bottom() : other.bottom();
        if (nr <= nx || nb <= ny) return {};
        return {nx, ny, nr - nx, nb - ny};
    }

    Rect insetBy(float dx, float dy) const {
        return {x + dx, y + dy, width - 2 * dx, height - 2 * dy};
    }

    Rect translated(const Point& offset) const {
        return {x + offset.x, y + offset.y, width, height};
    }
};

struct EdgeInsets {
    float top = 0;
    float right = 0;
    float bottom = 0;
    float left = 0;

    static EdgeInsets all(float v) { return {v, v, v, v}; }
    float horizontal() const { return left + right; }
    float vertical() const { return top + bottom; }
};

struct Color {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.0f;

    static Color fromRGBA(float r, float g, float b, float a) { return {r, g, b, a}; }
    static Color fromRGB(float r, float g, float b) { return {r, g, b, 1.0f}; }
    static Color fromHex(uint32_t hex) {
        return {
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >> 8) & 0xFF) / 255.0f,
            (hex & 0xFF) / 255.0f,
            ((hex >> 24) & 0xFF) / 255.0f
        };
    }
};

enum class TextAlign { Left, Center, Right };

} // namespace ui

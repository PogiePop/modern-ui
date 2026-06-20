#pragma once

#include <array>
#include <memory>
#include <string>

#include <glad/glad.h>

#include "core/Types.hpp"

namespace ui {

class Texture;

struct GlyphMetrics {
    float x0, y0, x1, y1;    // pixel-space quad (relative to baseline origin)
    float s0, t0, s1, t1;    // texture coordinates
    float xAdvance;           // pixels to advance after this glyph
};

class Font {
public:
    Font();
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    Font(Font&&) noexcept;
    Font& operator=(Font&&) noexcept;

    bool loadFromFile(const char* path, float fontSize);

    const GlyphMetrics& getGlyph(char c) const;
    float measureText(const std::string& text) const;

    float ascent() const { return m_ascent; }
    float descent() const { return m_descent; }
    float lineHeight() const { return m_lineHeight; }
    float fontSize() const { return m_fontSize; }

    GLuint atlasTexture() const;
    int atlasWidth() const { return m_atlasWidth; }
    int atlasHeight() const { return m_atlasHeight; }

private:
    std::unique_ptr<Texture> m_atlas;
    int m_atlasWidth = 512;
    int m_atlasHeight = 512;
    std::array<GlyphMetrics, 95> m_glyphs; // ASCII 32..126
    float m_fontSize = 14;
    float m_ascent = 0;
    float m_descent = 0;
    float m_lineHeight = 0;

    static constexpr int FIRST_CHAR = 32;
    static constexpr int NUM_CHARS = 95; // 32..126 inclusive
};

} // namespace ui

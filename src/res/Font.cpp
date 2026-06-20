#include "Font.hpp"
#include "Texture.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <cstdio>
#include <vector>

namespace ui {

Font::Font() = default;

Font::~Font() = default;

Font::Font(Font&&) noexcept = default;
Font& Font::operator=(Font&&) noexcept = default;

bool Font::loadFromFile(const char* path, float fontSize) {
    // Read entire .ttf file
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Font: cannot open '%s'\n", path);
        return false;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::vector<unsigned char> ttfData(static_cast<size_t>(size));
    fread(ttfData.data(), 1, static_cast<size_t>(size), f);
    fclose(f);

    m_fontSize = fontSize;

    // Get the correct font offset (handles both .ttf and .ttc)
    int fontOffset = stbtt_GetFontOffsetForIndex(ttfData.data(), 0);
    if (fontOffset < 0) {
        fprintf(stderr, "Font: '%s' is not a valid font file\n", path);
        return false;
    }

    // Bake font bitmap (single-channel alpha)
    std::vector<unsigned char> bitmap(static_cast<size_t>(m_atlasWidth * m_atlasHeight), 0);
    std::vector<stbtt_bakedchar> baked(NUM_CHARS);

    int result = stbtt_BakeFontBitmap(
        ttfData.data(), fontOffset,
        fontSize,
        bitmap.data(), m_atlasWidth, m_atlasHeight,
        FIRST_CHAR, NUM_CHARS,
        baked.data());

    if (result <= 0) {
        fprintf(stderr, "Font: failed to bake '%s' at size %.0f\n", path, fontSize);
        return false;
    }

    // Convert single-channel alpha to RGBA (shader needs RGBA for correct color blending)
    std::vector<unsigned char> rgba(static_cast<size_t>(m_atlasWidth * m_atlasHeight * 4), 0);
    for (int i = 0; i < m_atlasWidth * m_atlasHeight; ++i) {
        rgba[i * 4 + 0] = 255;          // R = white
        rgba[i * 4 + 1] = 255;          // G = white
        rgba[i * 4 + 2] = 255;          // B = white
        rgba[i * 4 + 3] = bitmap[i];    // A = glyph alpha
    }

    // Create OpenGL texture (RGBA, glyph in alpha channel)
    m_atlas = std::make_unique<Texture>();
    m_atlas->createFromData(m_atlasWidth, m_atlasHeight, 4, rgba.data());

    // Convert baked chars to GlyphMetrics
    for (int i = 0; i < NUM_CHARS; ++i) {
        const auto& bc = baked[i];
        GlyphMetrics& gm = m_glyphs[i];
        gm.x0 = bc.xoff;
        gm.y0 = bc.yoff;
        gm.x1 = bc.xoff + bc.x1 - bc.x0;
        gm.y1 = bc.yoff + bc.y1 - bc.y0;
        gm.s0 = bc.x0 / (float)m_atlasWidth;
        gm.t0 = bc.y0 / (float)m_atlasHeight;
        gm.s1 = bc.x1 / (float)m_atlasWidth;
        gm.t1 = bc.y1 / (float)m_atlasHeight;
        gm.xAdvance = bc.xadvance;
    }

    // Get vertical metrics (use same offset)
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttfData.data(), fontOffset);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    float scale = stbtt_ScaleForPixelHeight(&info, fontSize);
    m_ascent = ascent * scale;
    m_descent = descent * scale;
    m_lineHeight = (ascent - descent + lineGap) * scale;

    printf("Font: loaded '%s' at %.0fpx, line height %.1f\n", path, fontSize, m_lineHeight);

    return true;
}

const GlyphMetrics& Font::getGlyph(char c) const {
    int idx = static_cast<int>(static_cast<unsigned char>(c)) - FIRST_CHAR;
    if (idx < 0 || idx >= NUM_CHARS) {
        // Return '?' glyph or space
        idx = ('?' - FIRST_CHAR);
        if (idx < 0 || idx >= NUM_CHARS) idx = 0;
    }
    return m_glyphs[idx];
}

float Font::measureText(const std::string& text) const {
    float width = 0;
    for (char c : text) {
        width += getGlyph(c).xAdvance;
    }
    return width;
}

GLuint Font::atlasTexture() const {
    return m_atlas ? m_atlas->handle() : 0;
}

} // namespace ui

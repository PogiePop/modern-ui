#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <stb_truetype.h>

#include "core/Types.hpp"

namespace ui {

class Texture;

struct GlyphMetrics {
    float x0, y0, x1, y1;
    float s0, t0, s1, t1;
    float xAdvance;
    int atlasPage = 0;
};

inline uint32_t decodeUtf8(const char*& p) {
    unsigned char c = (unsigned char)*p;
    if (c < 0x80) { p++; return c; }
    uint32_t cp = 0; int len = 0;
    if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; len = 1; }
    else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; len = 2; }
    else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; len = 3; }
    else { p++; return 0xFFFD; }
    p++;
    for (int i = 0; i < len; i++, p++) cp = (cp << 6) | ((unsigned char)*p & 0x3F);
    return cp;
}

inline std::string codepointToUtf8(uint32_t cp) {
    std::string s;
    if (cp < 0x80) s += (char)cp;
    else if (cp < 0x800) { s += (char)(0xC0|(cp>>6)); s += (char)(0x80|(cp&0x3F)); }
    else if (cp < 0x10000) { s += (char)(0xE0|(cp>>12)); s += (char)(0x80|((cp>>6)&0x3F)); s += (char)(0x80|(cp&0x3F)); }
    else if (cp < 0x110000) { s += (char)(0xF0|(cp>>18)); s += (char)(0x80|((cp>>12)&0x3F)); s += (char)(0x80|((cp>>6)&0x3F)); s += (char)(0x80|(cp&0x3F)); }
    return s;
}

class Font {
public:
    Font();
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    Font(Font&&) noexcept;
    Font& operator=(Font&&) noexcept;

    bool loadFromFile(const char* path, float fontSize);
    bool reload(float newFontSize); // reload same font at different size

    const GlyphMetrics* getGlyph(uint32_t codepoint);
    float measureText(const std::string& text);

    float ascent() const { return m_ascent; }
    float descent() const { return m_descent; }
    float lineHeight() const { return m_lineHeight; }
    float fontSize() const { return m_fontSize; }

    int atlasPageCount() const { return (int)m_atlases.size(); }
    GLuint atlasTexture(int page = 0) const;

    const std::string& fontPath() const { return m_fontPath; }

    void addFallback(Font* fallback) { m_fallback = fallback; }

    // Pre-warm: batch-rasterize a set of codepoints (non-blocking prep)
    void preWarm(const std::vector<uint32_t>& codepoints);

private:
    std::vector<std::unique_ptr<Texture>> m_atlases;
    static constexpr int ATLAS_SIZE = 1024;
    std::unordered_map<uint32_t, GlyphMetrics> m_glyphs;
    float m_fontSize = 14;
    float m_ascent = 0, m_descent = 0, m_lineHeight = 0;

    std::vector<unsigned char> m_ttfData;
    stbtt_fontinfo m_info;
    int m_fontOffset = 0;
    float m_scale = 1.0f;
    bool m_loaded = false;
    std::string m_fontPath;

    // Current atlas packing state
    int m_curPage = 0;
    int m_packX = 1, m_packY = 1;
    int m_packLineHeight = 0;

    Font* m_fallback = nullptr;

    void newAtlasPage();
    bool rasterizeGlyph(uint32_t cp, GlyphMetrics& out);
    void resetAtlases();
};

} // namespace ui

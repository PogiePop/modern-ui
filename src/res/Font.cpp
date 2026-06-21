#include "Font.hpp"
#include "Texture.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <cstdio>
#include <cstring>
#include <algorithm>

namespace ui {

Font::Font() = default;
Font::~Font() = default;
Font::Font(Font&&) noexcept = default;
Font& Font::operator=(Font&&) noexcept = default;

void Font::newAtlasPage() {
    std::vector<unsigned char> empty(static_cast<size_t>(ATLAS_SIZE * ATLAS_SIZE * 4), 0);
    auto tex = std::make_unique<Texture>();
    tex->createFromData(ATLAS_SIZE, ATLAS_SIZE, 4, empty.data());
    m_atlases.push_back(std::move(tex));
    m_curPage = (int)m_atlases.size() - 1;
    m_packX = 1; m_packY = 1; m_packLineHeight = 0;
}

void Font::resetAtlases() {
    m_atlases.clear();
    m_glyphs.clear();
    newAtlasPage();
}

bool Font::loadFromFile(const char* path, float fontSize) {
    FILE* f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Font: cannot open '%s'\n", path); return false; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    m_ttfData.resize(static_cast<size_t>(size));
    fread(m_ttfData.data(), 1, static_cast<size_t>(size), f);
    fclose(f);

    m_fontOffset = stbtt_GetFontOffsetForIndex(m_ttfData.data(), 0);
    if (m_fontOffset < 0) { fprintf(stderr, "Font: '%s' is not valid\n", path); return false; }

    m_fontSize = fontSize;
    m_fontPath = path;
    stbtt_InitFont(&m_info, m_ttfData.data(), m_fontOffset);
    m_scale = stbtt_ScaleForPixelHeight(&m_info, fontSize);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&m_info, &ascent, &descent, &lineGap);
    m_ascent = ascent * m_scale;
    m_descent = descent * m_scale;
    m_lineHeight = (ascent - descent + lineGap) * m_scale;

    resetAtlases();
    m_loaded = true;

    // Pre-bake ASCII
    for (uint32_t cp = 32; cp < 127; cp++) getGlyph(cp);

    printf("Font: '%s' %.0fpx lineH=%.1f atlasPages=%d\n", path, fontSize, m_lineHeight, (int)m_atlases.size());
    return true;
}

bool Font::reload(float newFontSize) {
    if (m_fontPath.empty()) return false;
    return loadFromFile(m_fontPath.c_str(), newFontSize);
}

void Font::preWarm(const std::vector<uint32_t>& codepoints) {
    for (auto cp : codepoints) getGlyph(cp);
}

bool Font::rasterizeGlyph(uint32_t cp, GlyphMetrics& out) {
    if (!m_loaded) return false;

    int glyphIdx = stbtt_FindGlyphIndex(&m_info, (int)cp);
    if (glyphIdx == 0) return false;

    int advWidth, lsb;
    stbtt_GetGlyphHMetrics(&m_info, glyphIdx, &advWidth, &lsb);
    out.xAdvance = advWidth * m_scale;

    int x0, y0, x1, y1;
    stbtt_GetGlyphBitmapBox(&m_info, glyphIdx, m_scale, m_scale, &x0, &y0, &x1, &y1);
    int gw = x1 - x0, gh = y1 - y0;

    if (gw <= 0 || gh <= 0) {
        out.x0=out.y0=out.x1=out.y1=0; out.s0=out.t0=out.s1=out.t1=0; out.atlasPage=0;
        return true;
    }

    // Row wrap
    if (m_packX + gw + 1 >= ATLAS_SIZE) {
        m_packX = 1;
        m_packY += m_packLineHeight + 1;
        m_packLineHeight = 0;
    }
    // Need new atlas page?
    if (m_packY + gh + 1 >= ATLAS_SIZE) {
        newAtlasPage();
    }

    // Rasterize
    std::vector<unsigned char> glyphBitmap(static_cast<size_t>(gw * gh), 0);
    stbtt_MakeGlyphBitmap(&m_info, glyphBitmap.data(), gw, gh, gw, m_scale, m_scale, glyphIdx);

    GLuint tex = m_atlases[m_curPage]->handle();
    glBindTexture(GL_TEXTURE_2D, tex);

    std::vector<unsigned char> rgba(static_cast<size_t>(gw * gh * 4), 0);
    for (int i = 0; i < gw * gh; i++) {
        rgba[i*4+0]=255; rgba[i*4+1]=255; rgba[i*4+2]=255; rgba[i*4+3]=glyphBitmap[i];
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, m_packX, m_packY, gw, gh, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    out.x0=(float)x0; out.y0=(float)y0; out.x1=(float)x1; out.y1=(float)y1;
    out.s0=(float)m_packX/(float)ATLAS_SIZE;
    out.t0=(float)m_packY/(float)ATLAS_SIZE;
    out.s1=(float)(m_packX+gw)/(float)ATLAS_SIZE;
    out.t1=(float)(m_packY+gh)/(float)ATLAS_SIZE;
    out.atlasPage = m_curPage;

    m_packX += gw + 1;
    if (gh > m_packLineHeight) m_packLineHeight = gh;

    return true;
}

const GlyphMetrics* Font::getGlyph(uint32_t codepoint) {
    auto it = m_glyphs.find(codepoint);
    if (it != m_glyphs.end()) return &it->second;

    GlyphMetrics gm{};
    if (rasterizeGlyph(codepoint, gm)) {
        m_glyphs[codepoint] = gm;
        return &m_glyphs[codepoint];
    }

    if (m_fallback) return m_fallback->getGlyph(codepoint);

    auto qit = m_glyphs.find((uint32_t)'?');
    if (qit != m_glyphs.end()) return &qit->second;
    return nullptr;
}

float Font::measureText(const std::string& text) {
    float width = 0;
    const char* p = text.c_str();
    while (*p) {
        uint32_t cp = decodeUtf8(p);
        auto* gm = getGlyph(cp);
        if (gm) width += gm->xAdvance;
    }
    return width;
}

GLuint Font::atlasTexture(int page) const {
    if (page >= 0 && page < (int)m_atlases.size()) return m_atlases[page]->handle();
    return 0;
}

} // namespace ui

#include "Painter.hpp"
#include "QuadBatcher.hpp"
#include "res/Font.hpp"

#include <cmath>

namespace ui {

Painter::Painter(QuadBatcher& batcher, int fbWidth, int fbHeight)
    : m_batcher(batcher), m_fbWidth(fbWidth), m_fbHeight(fbHeight) {}

void Painter::begin(const glm::mat4& projection) {
    m_batcher.begin(projection);
}

void Painter::end() {
    m_batcher.end();
}

void Painter::drawRect(const Rect& rect, const Color& fill) {
    m_batcher.drawRect(rect, fill);
}

void Painter::drawGradientRect(const Rect& rect, const Color& top, const Color& bottom) {
    m_batcher.drawGradientRect(rect, top, bottom);
}

void Painter::drawRectOutline(const Rect& rect, const Color& stroke, float thickness) {
    // Top
    m_batcher.drawRect({rect.x, rect.y, rect.width, thickness}, stroke);
    // Bottom
    m_batcher.drawRect({rect.x, rect.y + rect.height - thickness, rect.width, thickness}, stroke);
    // Left
    m_batcher.drawRect({rect.x, rect.y, thickness, rect.height}, stroke);
    // Right
    m_batcher.drawRect({rect.x + rect.width - thickness, rect.y, thickness, rect.height}, stroke);
}

void Painter::drawText(const Rect& bounds, const std::string& text,
                       Font& font, const Color& color, TextAlign align) {
    if (text.empty()) return;

    GLuint tex = font.atlasTexture();
    if (!tex) return;

    // Measure total width for alignment
    float totalWidth = font.measureText(text);

    // Starting x position
    float startX = bounds.x;
    if (align == TextAlign::Center) {
        startX = bounds.x + (bounds.width - totalWidth) * 0.5f;
    } else if (align == TextAlign::Right) {
        startX = bounds.x + bounds.width - totalWidth;
    }

    // Vertical: center within bounds
    float baseline = bounds.y + (bounds.height - font.lineHeight()) * 0.5f + font.ascent();

    float penX = startX;
    for (char c : text) {
        const auto& g = font.getGlyph(c);

        // Don't draw spaces, just advance
        if (c != ' ') {
            float x0 = penX + g.x0;
            float y0 = baseline + g.y0;
            float x1 = penX + g.x1;
            float y1 = baseline + g.y1;

            m_batcher.drawTexturedRect(
                {x0, y0, x1 - x0, y1 - y0},
                g.s0, g.t0, g.s1, g.t1,
                color, tex);
        }

        penX += g.xAdvance;
    }
}

void Painter::drawText(const Rect& bounds, const std::string& text,
                       const Color& color, TextAlign align) {
    if (m_font) {
        drawText(bounds, text, *m_font, color, align);
    }
}

float Painter::measureTextWidth(const std::string& text) const {
    if (m_font) return m_font->measureText(text);
    return text.size() * 8.0f; // fallback
}

void Painter::drawImage(const Rect& rect, GLuint texture, const Color& tint) {
    m_batcher.drawTexturedRect(rect, 0, 0, 1, 1, tint, texture);
}

void Painter::pushClip(const Rect& rect) {
    m_batcher.pushClip(rect, m_fbHeight);
}

void Painter::popClip() {
    m_batcher.popClip(m_fbHeight);
}

void Painter::drawRoundedRect(const Rect& rect, const Color& fill, float radius) {
    m_batcher.drawRoundedRect(rect, fill, radius);
}

void Painter::drawRoundedGradientRect(const Rect& rect, const Color& top, const Color& bottom, float radius) {
    m_batcher.drawRoundedGradientRect(rect, top, bottom, radius);
}

void Painter::drawShadow(const Rect& rect, float radius, const Color& color, Point offset, float blurRadius) {
    if (blurRadius <= 0) blurRadius = 6;
    int passes = 4;
    for (int i = passes; i >= 0; --i) {
        float t = (float)i / passes;
        float expand = blurRadius * t;
        float alpha = color.a * (1.0f - t) / (passes + 1);
        Color layer{color.r, color.g, color.b, alpha};
        Rect sr{rect.x + offset.x - expand, rect.y + offset.y - expand,
                 rect.width + expand * 2, rect.height + expand * 2};
        m_batcher.drawRoundedRect(sr, layer, radius + expand);
    }
}

} // namespace ui

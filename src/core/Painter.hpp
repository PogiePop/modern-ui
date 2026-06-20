#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

#include "core/Types.hpp"

namespace ui {

class QuadBatcher;
class Font;

class Painter {
public:
    Painter(QuadBatcher& batcher, int fbWidth, int fbHeight);

    void begin(const glm::mat4& projection);
    void end();

    // Drawing
    void drawRect(const Rect& rect, const Color& fill);
    void drawGradientRect(const Rect& rect, const Color& top, const Color& bottom);
    void drawRectOutline(const Rect& rect, const Color& stroke, float thickness);
    void drawRoundedRect(const Rect& rect, const Color& fill, float radius);
    void drawRoundedGradientRect(const Rect& rect, const Color& top, const Color& bottom, float radius);

    // Shadow
    void drawShadow(const Rect& rect, float radius, const Color& color, Point offset, float blurRadius);

    // Text
    void drawText(const Rect& bounds, const std::string& text,
                  Font& font, const Color& color,
                  TextAlign align = TextAlign::Left);

    // Convenience: draw text using the default font set via setFont()
    void drawText(const Rect& bounds, const std::string& text,
                  const Color& color, TextAlign align = TextAlign::Left);

    // Set the default font
    void setFont(Font* font) { m_font = font; }
    Font* font() const { return m_font; }

    void setCursorVisible(bool v) { m_cursorVisible = v; }
    bool cursorVisible() const { return m_cursorVisible; }

    // Text measurement using loaded font
    float measureTextWidth(const std::string& text) const;

    // Image
    void drawImage(const Rect& rect, GLuint texture, const Color& tint);

    // Clipping
    void pushClip(const Rect& rect);
    void popClip();

    // Update framebuffer size (call on resize)
    void setFramebufferSize(int w, int h) { m_fbWidth = w; m_fbHeight = h; }

private:
    QuadBatcher& m_batcher;
    int m_fbWidth, m_fbHeight;
    Font* m_font = nullptr;
    bool m_cursorVisible = true;
};

} // namespace ui

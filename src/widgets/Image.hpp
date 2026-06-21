#pragma once

#include <glad/glad.h>

#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class Image : public Widget {
public:
    Image();

    void setTexture(GLuint texture, int texWidth, int texHeight);
    void setTint(const Color& tint) { m_tint = tint; }

    enum class ScaleMode { Stretch, Fit, Fill, None };
    void setScaleMode(ScaleMode mode) { m_scaleMode = mode; }
    void setImageRadius(float r) { m_radius = r; m_hasRadius = true; }
    void setImageShadow(Point off, float blur, const Color& c) { m_shadowOff=off; m_shadowBlur=blur; m_shadowCol=c; m_hasShadow=true; }

    const char* typeName() const override { return "Image"; }

    // Overrides
    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;

private:
    GLuint m_texture = 0;
    int m_texWidth = 0;
    int m_texHeight = 0;
    ScaleMode m_scaleMode = ScaleMode::Stretch;
    Color m_tint{1, 1, 1, 1};
    float m_radius = 0; bool m_hasRadius = false;
    Point m_shadowOff{0,4}; float m_shadowBlur = 12; Color m_shadowCol{0,0,0,0.3f}; bool m_hasShadow = false;
};

} // namespace ui

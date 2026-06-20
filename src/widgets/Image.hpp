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
};

} // namespace ui

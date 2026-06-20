#include "Image.hpp"
#include "core/Painter.hpp"

namespace ui {

Image::Image() {
    m_focusable = false;
}

void Image::setTexture(GLuint texture, int texWidth, int texHeight) {
    m_texture = texture;
    m_texWidth = texWidth;
    m_texHeight = texHeight;
}

Size Image::measure(const Size& available) const {
    if (m_texWidth <= 0 || m_texHeight <= 0) return {0, 0};
    if (m_scaleMode == ScaleMode::Stretch || m_scaleMode == ScaleMode::Fill) {
        return {available.width, available.height};
    }
    return {static_cast<float>(m_texWidth), static_cast<float>(m_texHeight)};
}

void Image::paint(Painter& painter) {
    if (!m_texture) return;
    painter.drawImage(screenRect(), m_texture, m_tint);
}

} // namespace ui

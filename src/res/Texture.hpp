#pragma once

#include <glad/glad.h>

namespace ui {

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) noexcept;
    Texture& operator=(Texture&&) noexcept;

    bool loadFromFile(const char* path);
    bool createFromData(int width, int height, int channels, const unsigned char* data);
    bool createEmpty(int width, int height, int channels);

    void bind(int unit = 0) const;

    GLuint handle() const { return m_handle; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    bool valid() const { return m_handle != 0; }

private:
    GLuint m_handle = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};

} // namespace ui

#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdio>

namespace ui {

Texture::~Texture() {
    if (m_handle) {
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_handle(other.m_handle)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_channels(other.m_channels) {
    other.m_handle = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_handle) glDeleteTextures(1, &m_handle);
        m_handle = other.m_handle; other.m_handle = 0;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
    }
    return *this;
}

bool Texture::loadFromFile(const char* path) {
    int w, h, c;
    unsigned char* data = stbi_load(path, &w, &h, &c, 4); // force RGBA
    if (!data) {
        fprintf(stderr, "Texture: failed to load '%s'\n", path);
        return false;
    }
    bool ok = createFromData(w, h, 4, data);
    stbi_image_free(data);
    return ok;
}

bool Texture::createFromData(int width, int height, int channels, const unsigned char* data) {
    if (m_handle) {
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
    }

    m_width = width;
    m_height = height;
    m_channels = channels;

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);

    GLint internalFormat = (channels == 1) ? GL_RED :
                           (channels == 4) ? GL_RGBA : GL_RGBA;
    GLenum format = (channels == 1) ? GL_RED :
                    (channels == 4) ? GL_RGBA : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Texture::createEmpty(int width, int height, int channels) {
    m_width = width;
    m_height = height;
    m_channels = channels;

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);

    GLint internalFormat = (channels == 1) ? GL_RED :
                           (channels == 4) ? GL_RGBA : GL_RGBA;
    GLenum format = (channels == 1) ? GL_RED :
                    (channels == 4) ? GL_RGBA : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                 format, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

} // namespace ui

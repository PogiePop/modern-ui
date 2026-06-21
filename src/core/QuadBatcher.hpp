#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>

#include "core/Types.hpp"
#include <glm/glm.hpp>

namespace ui {

class Shader;

struct QuadVertex {
    float x, y;
    float u, v;
    float r, g, b, a;
};

class QuadBatcher {
public:
    QuadBatcher();
    ~QuadBatcher();

    QuadBatcher(const QuadBatcher&) = delete;
    QuadBatcher& operator=(const QuadBatcher&) = delete;

    bool init();
    void shutdown();

    void begin(const glm::mat4& projection);
    void end();

    void drawRect(const Rect& rect, const Color& color);
    void drawGradientRect(const Rect& rect, const Color& topColor, const Color& bottomColor);
    void drawTexturedRect(const Rect& dstRect, float u0, float v0, float u1, float v1,
                          const Color& tint, GLuint texture);
    void drawRoundedRect(const Rect& rect, const Color& color, float radius, int segments = 16);
    void drawRoundedGradientRect(const Rect& rect, const Color& topColor, const Color& bottomColor,
                                 float radius, int segments = 16);
    void flush();
    void bindTexture(GLuint texture);

    void pushClip(const Rect& rect, int fbHeight);
    void popClip(int fbHeight);

    // Statistics
    size_t flushCount() const { return m_flushCount; }
    size_t drawCallCount() const { return m_drawCallCount; }
    void resetStats() { m_flushCount = 0; m_drawCallCount = 0; }

private:
    void createWhiteTexture();
    void setupVAO();
    void addQuad(const Rect& rect, float u0, float v0, float u1, float v1,
                 float r, float g, float b, float a);

    std::vector<QuadVertex> m_vertices;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_roundedShader;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_whiteTexture = 0;
    GLuint m_currentTexture = 0;
    glm::mat4 m_projection{1.0f};
    int m_fbHeight = 0;

    // GL state cache to avoid redundant calls (EUI-NEO pattern)
    GLuint m_boundProgram = 0;
    GLuint m_boundVAO = 0;
    GLuint m_boundTexture = 0;
    bool m_scissorEnabled = false;
    GLint m_scissorX = 0, m_scissorY = 0;
    GLsizei m_scissorW = 0, m_scissorH = 0;
    bool m_blendEnabled = true;

    // Clip stack — saves previous scissor state for proper nesting
    struct ClipState { GLint x, y; GLsizei w, h; bool enabled; };
    std::vector<ClipState> m_clipStack;

    size_t m_flushCount = 0;
    size_t m_drawCallCount = 0;
};

} // namespace ui

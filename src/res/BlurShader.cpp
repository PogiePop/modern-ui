#include "BlurShader.hpp"
#include "Shader.hpp"
#include <cstdio>

namespace ui {

static const char* kVtx = R"(#version 330 core
in vec2 aPos; in vec2 aUV; out vec2 vUV;
void main() { gl_Position = vec4(aPos, 0, 1); vUV = aUV; }
)";

static const char* kFrag = R"(#version 330 core
in vec2 vUV; out vec4 oColor;
uniform sampler2D uTex;
uniform vec2 uDir; // blur direction (1/w, 0) or (0, 1/h)
uniform float uRadius;
void main() {
    vec2 off = uDir * uRadius;
    vec4 c = texture(uTex, vUV) * 0.227027;
    c += texture(uTex, vUV + off*1.384615) * 0.316216;
    c += texture(uTex, vUV - off*1.384615) * 0.316216;
    c += texture(uTex, vUV + off*3.230769) * 0.070270;
    c += texture(uTex, vUV - off*3.230769) * 0.070270;
    oColor = c;
}
)";

BlurRenderer::BlurRenderer() = default;
BlurRenderer::~BlurRenderer() { shutdown(); }

bool BlurRenderer::init(int w, int h) {
    m_width = w; m_height = h;
    Shader s; if (!s.compile(kVtx, kFrag)) return false;
    m_blurShader = std::make_unique<Shader>(std::move(s));

    // Create textures
    glGenTextures(1, &m_colorTex); glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &m_blurTex); glBindTexture(GL_TEXTURE_2D, m_blurTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &m_fbo);
    setupQuad();
    return true;
}

void BlurRenderer::shutdown() {
    if (m_colorTex) glDeleteTextures(1, &m_colorTex);
    if (m_blurTex) glDeleteTextures(1, &m_blurTex);
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    m_colorTex = m_blurTex = m_fbo = m_vbo = m_vao = 0;
    m_blurShader.reset();
}

void BlurRenderer::setupQuad() {
    float q[] = {-1,1,0,1, -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,-1,1,0, 1,1,1,1};
    glGenVertexArrays(1, &m_vao); glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(q), q, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*4,(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*4,(void*)8);
    glBindVertexArray(0);
}

void BlurRenderer::resize(int w, int h) {
    m_width = w; m_height = h;
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_blurTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
}

void BlurRenderer::captureAndBlur(int w, int h) {
    if (w != m_width || h != m_height) resize(w, h);
    // Copy framebuffer to colorTex
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_colorTex);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
    // Blur: colorTex → blurTex (horizontal), then blurTex → colorTex (vertical)
    blurPass(m_colorTex, m_blurTex, w, h, true);
    blurPass(m_blurTex, m_colorTex, w, h, false);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void BlurRenderer::blurPass(GLuint src, GLuint dst, int w, int h, bool horizontal) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst, 0);
    glViewport(0, 0, w, h);
    m_blurShader->bind();
    m_blurShader->setInt("uTex", 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, src);
    float dirX = horizontal ? 1.5f/w : 0, dirY = horizontal ? 0 : 1.5f/h;
    GLint dLoc = glGetUniformLocation(m_blurShader->program(), "uDir");
    GLint rLoc = glGetUniformLocation(m_blurShader->program(), "uRadius");
    if (dLoc != -1) glUniform2f(dLoc, dirX, dirY);
    if (rLoc != -1) glUniform1f(rLoc, 1.0f);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void BlurRenderer::drawBlurredRect(float x, float y, float w, float h, float alpha) {
    // Draw the blurred texture in screen space
    glUseProgram(m_blurShader->program());
    m_blurShader->setInt("uTex", 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_colorTex);
    // Use the standard projection to position the quad
    // For simplicity, just draw a fullscreen quad tinted with alpha
    // The caller should set up scissor for the rect
    GLint aLoc = glGetUniformLocation(m_blurShader->program(), "uRadius");
    if (aLoc != -1) glUniform1f(aLoc, 0); // no extra blur, just sample
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);
    (void)alpha; // TODO: apply alpha via blend
}

} // namespace ui

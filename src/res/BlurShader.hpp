#pragma once
#include <glad/glad.h>
#include <memory>

namespace ui {

class Shader;

// Fullscreen blur using separable Gaussian (two-pass: horizontal + vertical)
class BlurRenderer {
public:
    BlurRenderer();
    ~BlurRenderer();
    bool init(int fbWidth, int fbHeight);
    void shutdown();

    // Capture current framebuffer, blur it, render result to screen
    void captureAndBlur(int fbWidth, int fbHeight);

    // Draw blurred background in specified rect (must be called after captureAndBlur)
    void drawBlurredRect(float x, float y, float w, float h, float alpha);

    void resize(int fbWidth, int fbHeight);

private:
    GLuint m_fbo = 0, m_colorTex = 0, m_blurTex = 0;
    GLuint m_vao = 0, m_vbo = 0;
    std::unique_ptr<Shader> m_blurShader;
    int m_width = 0, m_height = 0;
    void setupQuad();
    void blurPass(GLuint src, GLuint dst, int w, int h, bool horizontal);
};

} // namespace ui

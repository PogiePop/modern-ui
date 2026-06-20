#include "DialogWindow.hpp"
#include "QuadBatcher.hpp"
#include "Painter.hpp"
#include "core/Event.hpp"
#include "widgets/Widget.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

namespace ui {

DialogWindow::DialogWindow() = default;
DialogWindow::~DialogWindow() { close(); }

bool DialogWindow::open(const char* title, int w, int h, std::unique_ptr<Widget> content, Font* font, GLFWwindow* shareCtx) {
    if (m_handle) close();
    m_w = w; m_h = h; m_font = font; m_content = std::move(content);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_handle = glfwCreateWindow(w, h, title, nullptr, shareCtx);
    if (!m_handle) { fprintf(stderr, "DialogWindow: failed to create\n"); return false; }

    glfwMakeContextCurrent(m_handle);
    // glad already loaded by main window; shared context should inherit GL functions
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_batcher = std::make_unique<QuadBatcher>();
    if (!m_batcher->init()) { fprintf(stderr, "DialogWindow: batcher failed\n"); close(); return false; }
    m_painter = std::make_unique<Painter>(*m_batcher, w, h);
    if (m_font) m_painter->setFont(m_font);
    glfwShowWindow(m_handle);

    // Measure and layout content
    Size ms = m_content->measure({(float)w, (float)h});
    m_content->setBounds({0, 0, (float)w, (float)h});
    m_content->layout();

    return true;
}

void DialogWindow::close() {
    if (m_batcher) { m_batcher->shutdown(); m_batcher.reset(); }
    m_painter.reset(); m_content.reset();
    if (m_handle) { glfwDestroyWindow(m_handle); m_handle = nullptr; }
}

void DialogWindow::poll() {
    if (!m_handle) return;
    if (glfwWindowShouldClose(m_handle)) {
        if (onResult) onResult("");
        close(); return;
    }
    // Process input for this window
    glfwMakeContextCurrent(m_handle);

    // Mouse events
    double mx, my; int mbState = glfwGetMouseButton(m_handle, GLFW_MOUSE_BUTTON_LEFT);
    glfwGetCursorPos(m_handle, &mx, &my);
    static double lastMx = mx, lastMy = my;
    static int lastMb = 0;
    if (m_content) {
        if (mbState == GLFW_PRESS && lastMb == GLFW_RELEASE) {
            // Mouse down
            Point lp{(float)mx, (float)my};
            MouseEvent me{}; me.localPos = lp; me.button = 0; me.type = MouseEvent::Down;
            Widget* hit = m_content->hitTest(lp);
            if (hit) hit->onMouseDown(me);
        }
        lastMb = mbState; lastMx = mx; lastMy = my;
    }

    render();
}

void DialogWindow::render() {
    if (!m_handle || !m_content) return;
    glViewport(0, 0, m_w, m_h);
    glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto proj = glm::ortho(0.0f, (float)m_w, (float)m_h, 0.0f, -1.0f, 1.0f);
    m_painter->begin(proj);
    m_painter->setCursorVisible(true);
    m_content->paint(*m_painter);
    m_painter->end();
    glfwSwapBuffers(m_handle);
}

} // namespace ui

#include "Window.hpp"
#include "Types.hpp"
#include "App.hpp"
#include "core/Event.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>

namespace ui {

// --- Static GLFW callback forwarders ---

static void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onKey(key, scancode, action, mods);
}

static void charCallback(GLFWwindow* win, unsigned int codepoint) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onChar(codepoint);
}

static void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onMouseButton(button, action, mods);
}

static void cursorPosCallback(GLFWwindow* win, double x, double y) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onCursorPos(x, y);
}

static void scrollCallback(GLFWwindow* win, double dx, double dy) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onScroll(dx, dy);
}

static void framebufferSizeCallback(GLFWwindow* win, int width, int height) {
    auto* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
    if (window) window->onFramebufferSize(width, height);
}

// --- Window ---

Window::~Window() {
    destroy();
}

bool Window::create(const char* title, int width, int height) {
    if (m_handle) return true;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_handle) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load OpenGL via glad\n");
        destroy();
        return false;
    }

    fprintf(stdout, "OpenGL %s, GLSL %s\n",
            glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    installCallbacks();

    return true;
}

void Window::installCallbacks() {
    if (!m_handle) return;
    glfwSetWindowUserPointer(m_handle, this);

    glfwSetKeyCallback(m_handle, keyCallback);
    glfwSetCharCallback(m_handle, charCallback);
    glfwSetMouseButtonCallback(m_handle, mouseButtonCallback);
    glfwSetCursorPosCallback(m_handle, cursorPosCallback);
    glfwSetScrollCallback(m_handle, scrollCallback);
    glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);
}

void Window::destroy() {
    if (m_handle) {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
        glfwTerminate();
    }
}

bool Window::shouldClose() const {
    return m_handle ? glfwWindowShouldClose(m_handle) : true;
}

void Window::setShouldClose(bool value) {
    if (m_handle) glfwSetWindowShouldClose(m_handle, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::swapBuffers() {
    if (m_handle) glfwSwapBuffers(m_handle);
}

Size Window::framebufferSize() const {
    int w = 0, h = 0;
    if (m_handle) glfwGetFramebufferSize(m_handle, &w, &h);
    return {static_cast<float>(w), static_cast<float>(h)};
}

float Window::contentScaleX() const {
    float s = 1.0f;
    if (m_handle) glfwGetWindowContentScale(m_handle, &s, nullptr);
    return s;
}

float Window::contentScaleY() const {
    float s = 1.0f;
    if (m_handle) glfwGetWindowContentScale(m_handle, nullptr, &s);
    return s;
}

// --- GLFW callback forwarders ---

void Window::onMouseButton(int button, int action, int mods) {
    if (!m_app) return;

    double x, y;
    glfwGetCursorPos(m_handle, &x, &y);

    // Scale cursor pos from window coords to framebuffer coords (fix DPI)
    auto fb = framebufferSize();
    int winW = 0, winH = 0;
    glfwGetWindowSize(m_handle, &winW, &winH);
    if (winW > 0 && winH > 0) {
        x = x * fb.width / winW;
        y = y * fb.height / winH;
    }

    MouseEvent event;
    event.globalPos = {static_cast<float>(x), static_cast<float>(y)};
    event.button = button;
    event.mods = mods;
    event.type = (action == GLFW_PRESS) ? MouseEvent::Down : MouseEvent::Up;

    m_app->pushEvent(event);
}

void Window::onCursorPos(double x, double y) {
    if (!m_app) return;

    // Scale to framebuffer coords
    auto fb = framebufferSize();
    int winW = 0, winH = 0;
    glfwGetWindowSize(m_handle, &winW, &winH);
    if (winW > 0 && winH > 0) {
        x = x * fb.width / winW;
        y = y * fb.height / winH;
    }

    MouseEvent event;
    event.type = MouseEvent::Move;
    event.globalPos = {static_cast<float>(x), static_cast<float>(y)};
    event.button = -1;
    event.mods = 0;

    m_app->pushEvent(event);
}

void Window::onScroll(double dx, double dy) {
    if (!m_app) return;

    double x, y;
    glfwGetCursorPos(m_handle, &x, &y);

    auto fb = framebufferSize();
    int winW = 0, winH = 0;
    glfwGetWindowSize(m_handle, &winW, &winH);
    if (winW > 0 && winH > 0) {
        x = x * fb.width / winW;
        y = y * fb.height / winH;
    }

    ScrollEvent event;
    event.globalPos = {static_cast<float>(x), static_cast<float>(y)};
    event.dx = static_cast<float>(dx);
    event.dy = static_cast<float>(dy);

    m_app->pushEvent(event);
}

void Window::onKey(int key, int scancode, int action, int mods) {
    if (!m_app) return;

    KeyEvent event;
    event.key = key;
    event.scancode = scancode;
    event.mods = mods;
    if (action == GLFW_PRESS) event.type = KeyEvent::Down;
    else if (action == GLFW_RELEASE) event.type = KeyEvent::Up;
    else event.type = KeyEvent::Repeat;

    m_app->pushEvent(event);
}

void Window::onChar(unsigned int codepoint) {
    if (!m_app) return;

    CharEvent event;
    event.codepoint = codepoint;

    m_app->pushEvent(event);
}

void Window::onFramebufferSize(int width, int height) {
    if (!m_app) return;

    ResizeEvent event;
    event.newSize = {static_cast<float>(width), static_cast<float>(height)};

    m_app->pushEvent(event);
}

} // namespace ui

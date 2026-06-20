#pragma once

#include <string>

struct GLFWwindow;

namespace ui {

struct Size;
class App;

class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool create(const char* title, int width, int height);
    void destroy();

    bool shouldClose() const;
    void setShouldClose(bool value);

    void swapBuffers();

    Size framebufferSize() const;
    float contentScaleX() const;
    float contentScaleY() const;

    GLFWwindow* handle() const { return m_handle; }

    // App callback wiring
    void setApp(App* app) { m_app = app; }

    void onMouseButton(int button, int action, int mods);
    void onCursorPos(double x, double y);
    void onScroll(double dx, double dy);
    void onKey(int key, int scancode, int action, int mods);
    void onChar(unsigned int codepoint);
    void onFramebufferSize(int width, int height);

private:
    GLFWwindow* m_handle = nullptr;
    App* m_app = nullptr;

    void installCallbacks();
};

} // namespace ui

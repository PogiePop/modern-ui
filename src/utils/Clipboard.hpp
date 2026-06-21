#pragma once
#include <string>

namespace ui {

// Clipboard access via GLFW (requires GLFWwindow*)
class Clipboard {
public:
    // Call once during App::init with the GLFW window handle
    static void setWindow(void* glfwWindow) { s_window = glfwWindow; }

    static std::string get();
    static void set(const std::string& text);
    static bool available() { return s_window != nullptr; }

private:
    static void* s_window;
};

} // namespace ui

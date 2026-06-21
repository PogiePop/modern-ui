#include "Clipboard.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

namespace ui {

void* Clipboard::s_window = nullptr;

std::string Clipboard::get() {
    if (!s_window) return "";
    const char* str = glfwGetClipboardString((GLFWwindow*)s_window);
    return str ? std::string(str) : std::string();
}

void Clipboard::set(const std::string& text) {
    if (!s_window) return;
    glfwSetClipboardString((GLFWwindow*)s_window, text.c_str());
}

} // namespace ui

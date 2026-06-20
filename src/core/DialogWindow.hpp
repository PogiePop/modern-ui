#pragma once
#include <memory>
#include <string>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ui {

class Widget;
class Font;
class QuadBatcher;
class Painter;

// A secondary popup window that renders its own widget tree
class DialogWindow {
public:
    DialogWindow();
    ~DialogWindow();

    bool open(const char* title, int w, int h, std::unique_ptr<Widget> content, Font* font, GLFWwindow* shareCtx = nullptr);
    void close();
    bool isOpen() const { return m_handle != nullptr; }
    void poll(); // call each frame from main loop

    // Callback when window closes (with optional result string)
    std::function<void(const std::string&)> onResult;

private:
    GLFWwindow* m_handle = nullptr;
    std::unique_ptr<QuadBatcher> m_batcher;
    std::unique_ptr<Painter> m_painter;
    std::unique_ptr<Widget> m_content;
    Font* m_font = nullptr;
    int m_w = 0, m_h = 0;

    void render();
};

} // namespace ui

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/Event.hpp"

namespace ui {

class Window;
class QuadBatcher;
class Painter;
class Font;
class Widget;
class AnimationScheduler;
class BlurRenderer;
class DialogWindow;

class App {
public:
    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

    Window& window();
    void* windowHandle(); // GLFWwindow* for context sharing
    void quit();

    // Widget tree
    void setRoot(std::unique_ptr<Widget> root);
    Widget* root() const { return m_root.get(); }

    // Font
    Font* getFont() const { return m_font.get(); }

    // Animation
    AnimationScheduler& animator() { return *m_animator; }

    // Overlay (popup widgets rendered on top of everything)
    void setOverlay(Widget* w) { m_overlay = w; }
    Widget* overlay() const { return m_overlay; }

    // Child dialog windows
    void addDialogWindow(std::unique_ptr<DialogWindow> dlg);
    std::vector<std::unique_ptr<DialogWindow>>& dialogs() { return m_dialogs; }

    // Focus
    void requestFocus(Widget* w);
    Widget* focusedWidget() const { return m_focusedWidget; }
    void clearFocus();

    // Cursor blink
    bool cursorVisible() const { return m_cursorBlink; }
    void setCursorBlink(bool v) { m_cursorBlink = v; }

private:
    void update();
    void render();
    void relayout();

    // Event processing
    void processEvents();
    void processMouseEvent(struct MouseEvent& event);
    void processScrollEvent(struct ScrollEvent& event);
    void processKeyEvent(struct KeyEvent& event);
    void processCharEvent(struct CharEvent& event);

    void pushEvent(Event ev);

    Widget* hitTest(Point globalPos) const;
    void fireFocusEvent(Widget* widget, FocusEvent::Type type);

    // State
    std::unique_ptr<Window> m_window;
    std::unique_ptr<QuadBatcher> m_batcher;
    std::unique_ptr<Painter> m_painter;
    std::unique_ptr<Font> m_font;
    std::unique_ptr<AnimationScheduler> m_animator;
    std::unique_ptr<BlurRenderer> m_blur;
    std::unique_ptr<Widget> m_root;
    std::vector<std::unique_ptr<DialogWindow>> m_dialogs;
    bool m_running = false;
    bool m_layoutDirty = true;

    // Event state
    std::vector<Event> m_eventQueue;
    Widget* m_focusedWidget = nullptr;
    Widget* m_hoveredWidget = nullptr;
    Widget* m_captureWidget = nullptr;  // pointer capture for drag
    Widget* m_overlay = nullptr;
    Point m_lastMousePos;
    bool m_cursorBlink = true;
    int m_frameCount = 0;
    int m_tooltipTimer = 0;
    Widget* m_tooltipTarget = nullptr;

    friend class Window;
};

} // namespace ui

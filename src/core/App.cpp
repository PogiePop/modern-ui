#include "App.hpp"
#include "Window.hpp"
#include "Types.hpp"
#include "QuadBatcher.hpp"
#include "Painter.hpp"
#include "Event.hpp"

#include "res/Font.hpp"
#include "res/BlurShader.hpp"
#include "DialogWindow.hpp"
#include "utils/Clipboard.hpp"
#include "utils/ThreadPool.hpp"
#include "core/Config.hpp"
#include "Animation.hpp"
#include "widgets/Widget.hpp"
#include "widgets/Container.hpp"
#include "widgets/Tooltip.hpp"
#include "widgets/Toast.hpp"
#include "utils/AsyncTask.hpp"

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

namespace ui {

App* App::s_instance = nullptr;

App::App() { s_instance = this; }

App::~App() {
    shutdown();
}

bool App::init(const char* title, int width, int height) {
    m_window = std::make_unique<Window>();
    if (!m_window->create(title, width, height)) {
        return false;
    }

    // Wire GLFW callbacks to App
    m_window->setApp(this);

    // Initialize clipboard with GLFW window handle
    Clipboard::setWindow(m_window->handle());

    m_batcher = std::make_unique<QuadBatcher>();
    if (!m_batcher->init()) {
        fprintf(stderr, "Failed to init QuadBatcher\n");
        return false;
    }

    auto fbSize = m_window->framebufferSize();
    m_painter = std::make_unique<Painter>(*m_batcher,
        static_cast<int>(fbSize.width), static_cast<int>(fbSize.height));

    m_animator = std::make_unique<AnimationScheduler>();

    m_blur = std::make_unique<BlurRenderer>();
    m_blur->init((int)fbSize.width, (int)fbSize.height);

    // Load default font, scaled by DPI
    float dpiScale = m_window->contentScaleY();
    if (dpiScale < 1.0f) dpiScale = 1.0f;
    float baseSize = 16.0f * dpiScale;

    m_font = std::make_unique<Font>();
    const char* fontPaths[] = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        nullptr
    };
    bool fontLoaded = false;
    for (int i = 0; fontPaths[i]; ++i) {
        if (m_font->loadFromFile(fontPaths[i], baseSize)) {
            fontLoaded = true;
            break;
        }
    }
    if (fontLoaded) {
        m_painter->setFont(m_font.get());
    } else {
        fprintf(stderr, "Warning: No font loaded, text won't render\n");
    }

    return true;
}

void App::run() {
    if (!m_window) {
        fprintf(stderr, "App::run() called before init()\n");
        return;
    }

    m_running = true;
    while (!m_window->shouldClose() && m_running) {
        glfwPollEvents();
        processEvents();
        update();
        render();

        // Poll child dialog windows
        for (auto it = m_dialogs.begin(); it != m_dialogs.end(); ) {
            (*it)->poll();
            if (!(*it)->isOpen()) it = m_dialogs.erase(it);
            else ++it;
        }
        // Restore main window GL context
        if (m_window && m_window->handle())
            glfwMakeContextCurrent(m_window->handle());
    }
}

void App::addDialogWindow(std::unique_ptr<DialogWindow> dlg) {
    m_dialogs.push_back(std::move(dlg));
}

void App::shutdown() {
    m_running = false;
    m_root.reset();
    m_painter.reset();
    if (m_batcher) {
        m_batcher->shutdown();
        m_batcher.reset();
    }
    if (m_window) {
        m_window->destroy();
        m_window.reset();
    }
}

void App::setRoot(std::unique_ptr<Widget> root) {
    m_root = std::move(root);
    m_layoutDirty = true;
}

Window& App::window() { return *m_window; }

void* App::windowHandle() { return m_window ? (void*)m_window->handle() : nullptr; }

void App::quit() {
    if (m_window) m_window->setShouldClose(true);
}

// ---- Event queue ----

void App::pushEvent(Event ev) {
    m_eventQueue.push_back(std::move(ev));
}

void App::processEvents() {
    // Process all events from the queue (just one frame's worth)
    // Take ownership to allow re-entrant pushes
    auto queue = std::move(m_eventQueue);
    m_eventQueue.clear();

    for (auto& ev : queue) {
        std::visit([this](auto& e) {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, MouseEvent>) {
                processMouseEvent(e);
            } else if constexpr (std::is_same_v<T, ScrollEvent>) {
                processScrollEvent(e);
            } else if constexpr (std::is_same_v<T, KeyEvent>) {
                processKeyEvent(e);
            } else if constexpr (std::is_same_v<T, CharEvent>) {
                processCharEvent(e);
            } else if constexpr (std::is_same_v<T, ResizeEvent>) {
                m_layoutDirty = true;
            } else if constexpr (std::is_same_v<T, FocusEvent>) {
                // Internal focus events
            }
        }, ev);
    }
}

// ---- Mouse events ----

void App::processMouseEvent(MouseEvent& event) {
    m_lastMousePos = event.globalPos;

    // Helper: compute local coordinates by accumulating parent bounds (browser model)
    auto computeLocal = [](Widget* w, Point global) -> Point {
        Point accum = w->bounds().topLeft();
        for (Widget* p = w->parent(); p; p = p->parent()) {
            accum.x += p->bounds().x - p->scrollOffsetX();
            accum.y += p->bounds().y - p->scrollOffsetY();
        }
        return {global.x - accum.x, global.y - accum.y};
    };

    // Overlay gets first priority
    if (m_overlay && m_overlay->visible()) {
        Rect osr = m_overlay->screenRect();
        event.localPos = event.globalPos;
        event.localPos.x -= osr.x;
        event.localPos.y -= osr.y;
        if (event.type == MouseEvent::Down) {
            if (m_overlay->onMouseDown(event)) return;
        } else if (event.type == MouseEvent::Up) {
            if (m_overlay->onMouseUp(event)) return;
        } else if (event.type == MouseEvent::Move) {
            m_overlay->onMouseMove(event);
        }
    }

    // If we have pointer capture, deliver directly to capture widget
    if (m_captureWidget && event.type != MouseEvent::Move) {
        event.localPos = computeLocal(m_captureWidget, event.globalPos);
        switch (event.type) {
        case MouseEvent::Down:
            m_captureWidget->onMouseDown(event);
            break;
        case MouseEvent::Up:
            m_captureWidget->onMouseUp(event);
            m_captureWidget = nullptr;
            break;
        default: break;
        }
        return;
    }

    // For Move events with capture, still deliver to capture
    if (m_captureWidget && event.type == MouseEvent::Move) {
        event.localPos = computeLocal(m_captureWidget, event.globalPos);
        m_captureWidget->onMouseMove(event);
        return;
    }

    // Hit test to find target
    Widget* target = hitTest(event.globalPos);
    if (event.type == MouseEvent::Down) {
        printf("[App] click global=(%.0f,%.0f) target=%s\n",
               event.globalPos.x, event.globalPos.y, target ? target->typeName() : "null");
    }

    // Handle Enter/Leave for the Move event
    if (event.type == MouseEvent::Move) {
        if (target != m_hoveredWidget) {
            // Leave old
            if (m_hoveredWidget) {
                MouseEvent leave = event;
                leave.type = MouseEvent::Leave;
                leave.localPos = leave.globalPos;
                leave.localPos.x -= m_hoveredWidget->globalPosition().x;
                leave.localPos.y -= m_hoveredWidget->globalPosition().y;
                m_hoveredWidget->onMouseLeave(leave);
            }
            // Enter new
            if (target) {
                MouseEvent enter = event;
                enter.type = MouseEvent::Enter;
                enter.localPos = enter.globalPos;
                enter.localPos.x -= target->globalPosition().x;
                enter.localPos.y -= target->globalPosition().y;
                target->onMouseEnter(enter);
            }
            m_hoveredWidget = target;
        }
    }

    // Dispatch event via bubble — accumulate parent bounds like hitTest (browser model)
    Widget* dispatcher = target;
    while (dispatcher) {
        event.localPos = computeLocal(dispatcher, event.globalPos);
        bool handled = false;
        switch (event.type) {
        case MouseEvent::Down:
            handled = dispatcher->onMouseDown(event);
            if (handled) {
                // Start pointer capture
                m_captureWidget = dispatcher;
                // Request focus on click
                requestFocus(dispatcher);
            }
            break;
        case MouseEvent::Up:
            handled = dispatcher->onMouseUp(event);
            break;
        case MouseEvent::Move:
            handled = dispatcher->onMouseMove(event);
            break;
        default: break;
        }

        if (handled) break;
        dispatcher = dispatcher->parent();
    }

    // If mouse down on empty area (no handler), clear focus
    if (event.type == MouseEvent::Down && !m_captureWidget) {
        clearFocus();
    }
}

// ---- Scroll events ----

void App::processScrollEvent(ScrollEvent& event) {
    // Overlay gets scroll first
    if (m_overlay && m_overlay->visible()) {
        m_overlay->onScrollWheel(event.dx, event.dy);
        return;
    }
    Widget* dispatcher = m_hoveredWidget ? m_hoveredWidget : hitTest(event.globalPos);
    while (dispatcher) {
        if (dispatcher->onScrollWheel(event.dx, event.dy)) break;
        dispatcher = dispatcher->parent();
    }
}

// ---- Key events ----

static void collectFocusable(Widget* widget, std::vector<Widget*>& out) {
    if (!widget || !widget->visible()) return;
    if (widget->focusable()) out.push_back(widget);
    for (size_t i = 0; i < widget->childCount(); ++i) {
        collectFocusable(widget->childAt(i), out);
    }
}

void App::processKeyEvent(KeyEvent& event) {
    // Tab navigation
    if (event.type == KeyEvent::Down && event.key == 258) { // GLFW_KEY_TAB
        bool shift = (event.mods & 0x0001) != 0; // GLFW_MOD_SHIFT

        // Collect all focusable widgets
        std::vector<Widget*> focusables;
        collectFocusable(m_root.get(), focusables);

        // Sort by tabIndex (stable sort preserves tree order for same tabIndex)
        std::stable_sort(focusables.begin(), focusables.end(),
            [](Widget* a, Widget* b) { return a->tabIndex() < b->tabIndex(); });

        if (focusables.empty()) return;

        // Find current index
        int currentIdx = -1;
        for (size_t i = 0; i < focusables.size(); ++i) {
            if (focusables[i] == m_focusedWidget) {
                currentIdx = static_cast<int>(i);
                break;
            }
        }

        // Move to next/prev
        int nextIdx;
        if (shift) {
            nextIdx = (currentIdx <= 0) ? static_cast<int>(focusables.size()) - 1 : currentIdx - 1;
        } else {
            nextIdx = (currentIdx + 1) % static_cast<int>(focusables.size());
        }

        requestFocus(focusables[nextIdx]);
        return;
    }

    // Deliver to focused widget with bubble
    Widget* dispatcher = m_focusedWidget;
    while (dispatcher) {
        if (dispatcher->onKeyDown(event)) break;
        dispatcher = dispatcher->parent();
    }
}

// ---- Char events ----

void App::processCharEvent(CharEvent& event) {
    // Deliver only to focused widget (no bubble)
    if (m_focusedWidget) {
        m_focusedWidget->onCharInput(event);
    }
}

// ---- Hit testing ----

Widget* App::hitTest(Point globalPos) const {
    // Overlay checked first
    if (m_overlay && m_overlay->visible()) {
        Point lp = globalPos;
        lp.x -= m_overlay->screenRect().x;
        lp.y -= m_overlay->screenRect().y;
        if (m_overlay->hitTest(lp)) return m_overlay;
    }
    if (!m_root) return nullptr;
    return m_root->hitTest(globalPos);
}

// ---- Focus ----

void App::requestFocus(Widget* w) {
    if (m_focusedWidget == w) return;

    if (m_focusedWidget) {
        m_focusedWidget->onFocusLost();
    }
    m_focusedWidget = w;
    if (m_focusedWidget) {
        m_focusedWidget->onFocusGained();
    }
}

void App::clearFocus() {
    if (m_focusedWidget) {
        m_focusedWidget->onFocusLost();
        m_focusedWidget = nullptr;
    }
}

void App::fireFocusEvent(Widget* widget, FocusEvent::Type type) {
    (void)widget; (void)type;
}

// ---- Update ----

void App::update() {
    float dt = 1.0f / 60.0f;
    m_animator->update(dt);

    // Process async tasks from background threads
    AsyncTaskQueue::instance().process();

    // Tick widget animations
    if (m_root) m_root->tickAnimation(dt);

    // Update toasts
    ToastManager::instance().update(dt);

    // Cursor blink: toggle every 30 frames
    ++m_frameCount;
    if (m_frameCount >= 30) { m_frameCount = 0; m_cursorBlink = !m_cursorBlink; }

    // Tooltip hover timer (~0.75s delay)
    auto& tm = TooltipManager::instance();
    if (m_hoveredWidget != m_tooltipTarget) { m_tooltipTarget = m_hoveredWidget; m_tooltipTimer = 0; tm.hide(); }
    if (m_tooltipTarget) {
        ++m_tooltipTimer;
        if (m_tooltipTimer == 45)
            tm.showFor(m_tooltipTarget, m_lastMousePos.x, m_lastMousePos.y);
    } else { tm.hide(); m_tooltipTimer = 0; }

    if ((m_layoutDirty || (m_root && m_root->isLayoutDirty())) && m_root) {
        relayout();
        m_layoutDirty = false;
        m_root->clearLayoutDirty();
    }
}

void App::setMultiThreaded(bool enable) {
    Config::instance().multiThreaded = enable;
    m_multiThreaded = enable;
    if (enable) ThreadPool::init(Config::instance().renderThreads);
}

void App::relayout() {
    if (!m_root) return;
    auto fbSize = m_window->framebufferSize();
    Size rootSize = m_root->measure(fbSize);
    m_root->setBounds({0, 0, fbSize.width, fbSize.height});

    if (m_multiThreaded && ThreadPool::instance().workerCount() > 1) {
        // Parallel measure first-level children for layout
        auto& pool = ThreadPool::instance();
        pool.parallelFor(0, m_root->childCount(), [this](size_t i) {
            auto* child = m_root->childAt(i);
            if (child->visible()) child->measure(m_root->bounds().size());
        });
    }
    m_root->layout();
}

// ---- Render ----

void App::render() {
    auto fbSize = m_window->framebufferSize();
    float w = fbSize.width;
    float h = fbSize.height;

    glViewport(0, 0, static_cast<int>(w), static_cast<int>(h));
    glClearColor(0.18f, 0.20f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update painter's framebuffer size (handles resize)
    m_painter->setFramebufferSize(static_cast<int>(w), static_cast<int>(h));
    m_painter->setCursorVisible(m_cursorBlink);

    auto proj = glm::ortho(0.0f, w, h, 0.0f, -1.0f, 1.0f);
    m_painter->begin(proj);

    if (m_root) {
        m_root->paint(*m_painter);
    }

    // Frosted glass: only blur if overlay explicitly requests it (frosted() flag)
    if (m_overlay && m_overlay->visible() && m_overlay->frosted()) {
        m_painter->end();
        m_blur->captureAndBlur((int)w, (int)h);
        m_painter->begin(proj);
    }

    // Paint overlay on top of everything
    if (m_overlay && m_overlay->visible()) {
        m_overlay->paint(*m_painter);
    }

    // Tooltip
    TooltipManager::instance().render(*m_painter, m_font.get());

    // Toast notifications (top of screen)
    ToastManager::instance().render(*m_painter, m_font.get(), w);

    m_painter->end();

    m_window->swapBuffers();
}

} // namespace ui

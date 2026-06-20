#pragma once

#include <functional>
#include <string>
#include <memory>

#include "Widget.hpp"
#include "widgets/Container.hpp"

namespace ui {

class Dialog : public Container {
public:
    Dialog();

    void setTitle(const std::string& title);
    const std::string& title() const { return m_title; }

    void show(Widget* parent);
    void close();

    bool isOpen() const { return m_open; }
    void setOnClose(std::function<void()> cb) { m_onClose = std::move(cb); }

    const char* typeName() const override { return "Dialog"; }

    void paint(Painter& painter) override;
    Widget* hitTest(Point localPoint) override;
    bool onMouseDown(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;

private:
    std::string m_title;
    bool m_open = false;
    std::function<void()> m_onClose;
    Widget* m_parentWidget = nullptr;
    float m_dragOffsetX = 0, m_dragOffsetY = 0;
    bool m_dragging = false;
    static constexpr float TITLE_BAR_H = 32;
};

} // namespace ui

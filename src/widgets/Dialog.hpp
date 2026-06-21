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
    void setDialogSize(float w, float h) { m_dlgW = w; m_dlgH = h; }
    void setBackdropAlpha(float a) { m_backdropAlpha = a; }
    void setCornerRadius(float r) { m_radius = r; }

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
    float m_dlgW = 400, m_dlgH = 300, m_backdropAlpha = 0.5f, m_radius = 8;
    static constexpr float TITLE_BAR_H = 32;
};

} // namespace ui

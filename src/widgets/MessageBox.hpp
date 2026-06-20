#pragma once
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include "Widget.hpp"

namespace ui {

class MessageBox : public Widget {
public:
    MessageBox();

    void setTitle(const std::string& title) { m_title = title; }
    void setMessage(const std::string& msg) { m_message = msg; }
    void setFont(class Font* f) { m_font = f; }
    void setIcon(IconType icon) { m_icon = icon; }
    void addButton(const std::string& label, std::function<void()> onClick);
    void clearButtons();

    void show(Widget* parent, float w = 360, float h = 200);
    void close();
    bool isOpen() const { return m_open; }

    const char* typeName() const override { return "MessageBox"; }
    Rect screenRect() const;
    Size measure(const Size&) const override { return {300, 120}; }
    void layout() override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseUp(MouseEvent& e) override;
    bool onKeyDown(KeyEvent& e) override;
    Widget* hitTest(Point p) override;

private:
    std::string m_title, m_message;
    bool m_open = false;
    Widget* m_parentWidget = nullptr;
    class Font* m_font = nullptr;
    IconType m_icon = IconType::None;
    float m_w = 360, m_h = 200;
    std::vector<std::string> wrapText(const std::string& text, float maxW) const;
};

} // namespace ui

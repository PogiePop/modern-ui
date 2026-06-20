#pragma once
#include <string>
#include "Widget.hpp"

namespace ui {

class CollapsePanel : public Widget {
public:
    explicit CollapsePanel(const std::string& title = "");

    void setTitle(const std::string& t) { m_title = t; }
    void setContent(std::unique_ptr<Widget> content);
    void setExpanded(bool e);
    bool expanded() const { return m_expanded; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "CollapsePanel"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;

private:
    std::string m_title;
    bool m_expanded = false;
    class Font* m_font = nullptr;
    static constexpr float HEADER_H = 30;
};

} // namespace ui

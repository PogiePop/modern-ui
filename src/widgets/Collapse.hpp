#pragma once
#include <string>
#include "Widget.hpp"
#include "res/Theme.hpp"

namespace ui {

class CollapsePanel : public Widget {
public:
    enum Variant { Default, Bordered, Card };

    explicit CollapsePanel(const std::string& title = "");

    void setTitle(const std::string& t) { m_title = t; }
    void setContent(std::unique_ptr<Widget> content);
    void setExpanded(bool e);
    bool expanded() const { return m_expanded; }
    void setFont(class Font* f) { m_font = f; }
    void setVariant(Variant v) { m_variant = v; }
    void setColorRole(ColorRole r) { m_colorRole = r; m_useColorRole = true; }

    const char* typeName() const override { return "CollapsePanel"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;

private:
    std::string m_title;
    bool m_expanded = false;
    class Font* m_font = nullptr;
    float m_headerH = 30;
    Variant m_variant = Default;
    ColorRole m_colorRole = ColorRole::Primary;
    bool m_useColorRole = false;
};

} // namespace ui

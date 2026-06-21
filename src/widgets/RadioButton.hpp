#pragma once
#include <functional>
#include <string>
#include <vector>
#include "Widget.hpp"
#include "res/Theme.hpp"

namespace ui {

class RadioGroup {
public:
    void add(class RadioButton* btn);
    void remove(class RadioButton* btn);
    class RadioButton* selected() const { return m_selected; }
    void select(class RadioButton* btn);
private:
    std::vector<class RadioButton*> m_buttons;
    class RadioButton* m_selected = nullptr;
};

class RadioButton : public Widget {
public:
    explicit RadioButton(const std::string& label = "", RadioGroup* group = nullptr);

    void setLabel(const std::string& label);
    const std::string& label() const { return m_label; }
    bool selected() const { return m_selected; }
    void setSelected(bool sel);
    void setOnSelectedChanged(std::function<void(bool)> cb);
    void setGroup(RadioGroup* group);
    void setFont(class Font* f) { m_font = f; }
    void setColorRole(ColorRole r) { m_colorRole = r; m_useColorRole = true; }
    void setDotSize(float s) { m_dotSize = s; }

    const char* typeName() const override { return "RadioButton"; }
    Size measure(const Size& available) const override;
    void paint(Painter& painter) override;
    bool onMouseDown(MouseEvent& event) override;
    bool onKeyDown(KeyEvent& event) override;
    bool onMouseEnter(MouseEvent& event) override;
    bool onMouseLeave(MouseEvent& event) override;
    void onFocusGained() override;
    void onFocusLost() override;

private:
    std::string m_label;
    bool m_selected = false;
    std::function<void(bool)> m_onSelectedChanged;
    RadioGroup* m_group = nullptr;
    bool m_hovered = false, m_focusRing = false;
    class Font* m_font = nullptr;
    float m_dotSize = 10.0f;
    ColorRole m_colorRole = ColorRole::Primary;
    bool m_useColorRole = false;

    friend class RadioGroup;
};

} // namespace ui

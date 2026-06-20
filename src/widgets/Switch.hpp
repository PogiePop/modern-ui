#pragma once
#include <functional>
#include "Widget.hpp"

namespace ui {

class Switch : public Widget {
public:
    Switch();
    void setChecked(bool c);
    bool checked() const { return m_checked; }
    void setOnChanged(std::function<void(bool)> cb) { m_onChanged = std::move(cb); }

    const char* typeName() const override { return "Switch"; }
    Size measure(const Size&) const override { return {44, 24}; }
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onKeyDown(KeyEvent& e) override;

private:
    bool m_checked = false;
    std::function<void(bool)> m_onChanged;
    float m_animPos = 0;
};

} // namespace ui

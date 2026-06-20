#pragma once
#include <functional>
#include "Widget.hpp"

namespace ui {

class InputNumber : public Widget {
public:
    InputNumber();

    void setValue(int v);
    int value() const { return m_value; }
    void setRange(int min, int max) { m_min = min; m_max = max; }
    void setStep(int s) { m_step = s; }
    void setOnValueChanged(std::function<void(int)> cb) { m_onChanged = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "InputNumber"; }
    Size measure(const Size&) const override { return {120, 30}; }
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onKeyDown(KeyEvent& e) override;
    bool onCharInput(CharEvent& e) override;
    void onFocusGained() override { m_focused = true; }
    void onFocusLost() override { m_focused = false; }

private:
    int m_value = 0, m_min = 0, m_max = 100, m_step = 1;
    std::function<void(int)> m_onChanged;
    class Font* m_font = nullptr;
    bool m_focused = false;
    void applyValue(int v);
};

} // namespace ui

#pragma once
#include <string>

namespace ui {

class Widget;

// Manages tooltip display
class TooltipManager {
public:
    static TooltipManager& instance();

    void setTooltip(Widget* target, const std::string& text);
    void clearTooltip(Widget* target);
    bool hasTooltip(Widget* target) const { return m_target == target && !m_text.empty(); }
    void showFor(Widget* target, float x, float y);
    bool isVisible() const { return m_visible; }

    void showAt(float x, float y);
    void hide();
    void render(class Painter& painter, class Font* font);

private:
    Widget* m_target = nullptr;
    std::string m_text;
    bool m_visible = false;
    float m_x = 0, m_y = 0;
};

// Convenience: set tooltip on any widget
void setTooltip(Widget* w, const std::string& text);

} // namespace ui

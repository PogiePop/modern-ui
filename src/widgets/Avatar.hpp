#pragma once
#include <string>
#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class Avatar : public Widget {
public:
    Avatar();

    void setName(const std::string& name);    // extracts initials
    void setInitials(const std::string& init); // custom initials (max 2 chars)
    void setSize(float diameter);
    void setColor(const Color& bg);
    void setTextColor(const Color& tc) { m_textColor = tc; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "Avatar"; }
    Size measure(const Size&) const override { return {m_diameter, m_diameter}; }
    void paint(Painter& painter) override;

private:
    std::string m_initials;
    float m_diameter = 40;
    Color m_bgColor{0.3f, 0.5f, 0.9f, 1};
    Color m_textColor{1, 1, 1, 1};
    class Font* m_font = nullptr;
};

} // namespace ui

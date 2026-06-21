#pragma once
#include <string>
#include "Widget.hpp"

namespace ui {

// Inline badge/tag — small rounded pill with text
class Badge : public Widget {
public:
    enum Variant { Default, Primary, Success, Danger, Warning, Info };

    explicit Badge(const std::string& text = "", Variant v = Default);

    void setText(const std::string& t) { m_text = t; }
    void setVariant(Variant v);
    void setColor(const Color& bg, const Color& text);

    const char* typeName() const override { return "Badge"; }
    Size measure(const Size&) const override;
    void paint(Painter& p) override;

private:
    std::string m_text;
    Variant m_variant = Default;
    Color m_bgColor{0.2f, 0.2f, 0.3f, 1};
    Color m_textColor{0.7f, 0.7f, 0.8f, 1};
    bool m_customColor = false;
};

} // namespace ui

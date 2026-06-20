#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "core/Types.hpp"
#include "res/Theme.hpp"

namespace ui {

class Widget;

// A single CSS-like rule
struct StyleRule {
    std::string selector;    // e.g. "Button", ".primary", "#submit", "Button.primary"
    ColorRole colorRole = ColorRole::Transparent;
    Color colorValue{0,0,0,0};
    FontSizeRole fontSizeRole = FontSizeRole::Body;
    float fontSizeValue = 0;
    SpacingRole spacingRole = SpacingRole::Md;
    float spacingValue = 0;
    bool hasColor = false;
    bool hasFontSize = false;
    bool hasSpacing = false;
    bool hasBgColor = false;
    Color bgColor{0,0,0,0};
};

class StyleSheet {
public:
    StyleSheet() = default;

    // Parse from a CSS-like string
    bool parse(const char* source);

    // Apply to widget tree
    void apply(Widget* root) const;

    // Add rule programmatically
    void addRule(const StyleRule& rule) { m_rules.push_back(rule); }
    void clearRules() { m_rules.clear(); }

private:
    std::vector<StyleRule> m_rules;

    bool matches(const Widget* widget, const std::string& selector) const;
    void applyToWidget(Widget* widget) const;
};

} // namespace ui

#include "StyleSheet.hpp"
#include "widgets/Widget.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace ui {

// Simple CSS parser
bool StyleSheet::parse(const char* source) {
    m_rules.clear();

    const char* p = source;
    while (*p) {
        // Skip whitespace
        while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) ++p;
        if (!*p) break;

        // Read selector until '{'
        std::string selector;
        while (*p && *p != '{' && *p != '\n' && *p != '\r') {
            if (*p != ' ' && *p != '\t') selector += *p;
            ++p;
        }
        if (!*p) break;

        // Skip past '{'
        if (*p == '{') ++p;

        StyleRule rule;
        rule.selector = selector;

        // Read properties until '}'
        while (*p && *p != '}') {
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ';')) ++p;
            if (!*p || *p == '}') break;

            // Read property name
            std::string prop;
            while (*p && *p != ':' && *p != ' ' && *p != '\t') { prop += *p; ++p; }
            while (*p && *p != ':') ++p;
            if (*p == ':') ++p;
            while (*p && (*p == ' ' || *p == '\t')) ++p;

            // Read value
            std::string val;
            while (*p && *p != ';' && *p != '}' && *p != '\n' && *p != '\r') {
                if (*p != ' ' && *p != '\t') val += *p;
                ++p;
            }
            if (*p == ';') ++p;

            // Map property to rule
            if (prop == "color") {
                rule.hasColor = true;
                // Parse hex color #RRGGBB or #RRGGBBAA
                if (!val.empty() && val[0] == '#') {
                    unsigned int hex = strtoul(val.c_str() + 1, nullptr, 16);
                    if (val.length() <= 7) hex = (hex << 8) | 0xFF;
                    rule.colorValue = Color::fromHex(hex);
                }
            } else if (prop == "background-color" || prop == "bg-color") {
                rule.hasBgColor = true;
                if (!val.empty() && val[0] == '#') {
                    unsigned int hex = strtoul(val.c_str() + 1, nullptr, 16);
                    if (val.length() <= 7) hex = (hex << 8) | 0xFF;
                    rule.bgColor = Color::fromHex(hex);
                }
            } else if (prop == "font-size") {
                rule.hasFontSize = true;
                rule.fontSizeValue = (float)atof(val.c_str());
            } else if (prop == "spacing" || prop == "padding") {
                rule.hasSpacing = true;
                rule.spacingValue = (float)atof(val.c_str());
            }
        }
        if (*p == '}') ++p;

        if (!rule.selector.empty()) m_rules.push_back(rule);
    }
    return true;
}

bool StyleSheet::matches(const Widget* widget, const std::string& selector) const {
    if (!widget) return false;

    // Type selector: "Button", "Label", etc.
    if (selector == widget->typeName()) return true;

    // ID selector: "#submit"
    if (!selector.empty() && selector[0] == '#' && selector.substr(1) == widget->id()) return true;

    // Class selector: ".primary"
    if (!selector.empty() && selector[0] == '.' && selector.substr(1) == widget->className()) return true;

    // Type.class: "Button.primary"
    auto dotPos = selector.find('.');
    if (dotPos != std::string::npos) {
        std::string typePart = selector.substr(0, dotPos);
        std::string classPart = selector.substr(dotPos + 1);
        return (typePart == widget->typeName() || typePart.empty()) &&
               classPart == widget->className();
    }

    return false;
}

void StyleSheet::applyToWidget(Widget* widget) const {
    if (!widget) return;

    for (const auto& rule : m_rules) {
        if (matches(widget, rule.selector)) {
            if (rule.hasColor) {
                widget->setStyleColor(rule.colorValue);
            }
            if (rule.hasBgColor) {
                widget->setStyleBgColor(rule.bgColor);
            }
            if (rule.hasFontSize) {
                widget->setStyleFontSize(rule.fontSizeValue);
            }
        }
    }

    // Recurse into children
    for (size_t i = 0; i < widget->childCount(); ++i) {
        applyToWidget(widget->childAt(i));
    }
}

void StyleSheet::apply(Widget* root) const {
    applyToWidget(root);
}

} // namespace ui

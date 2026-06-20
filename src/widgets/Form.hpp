#pragma once
#include <string>
#include "Widget.hpp"
#include "widgets/Container.hpp"

namespace ui {

// A form field: label + input widget
class FormField : public Container {
public:
    FormField(const std::string& label, std::unique_ptr<Widget> input);

    void setLabel(const std::string& t) { m_labelText = t; }
    const char* typeName() const override { return "FormField"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;
    void setFont(class Font* f) { m_font = f; }

private:
    std::string m_labelText;
    class Font* m_font = nullptr;
};

// A form container with labeled fields
class Form : public Container {
public:
    Form();
    void addField(const std::string& label, std::unique_ptr<Widget> input);
    void setFont(class Font* f);
    const char* typeName() const override { return "Form"; }
private:
    class Font* m_fontPtr = nullptr;
};

} // namespace ui

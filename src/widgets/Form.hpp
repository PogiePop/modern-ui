#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "Widget.hpp"
#include "res/Theme.hpp"

namespace ui {

class Font;
enum class FormLabelPos { Top, Left };

// Form: logical grouping of input fields with unified submit
class Form : public Widget {
public:
    Form();

    // Add a labeled input field
    void addField(const std::string& label, std::unique_ptr<Widget> input,
                  FormLabelPos pos = FormLabelPos::Top);
    // Add a submit button (auto-placed at the end)
    void setSubmitButton(const std::string& text, std::function<void()> onCustom = nullptr);
    // Collect all field values and print to console
    void submit();
    // Set spacing between fields
    void setSpacing(float s) { m_spacing = s; }
    void setPadding(const EdgeInsets& p) { m_padding = p; }
    void setFont(Font* f) { m_font = f; }

    const char* typeName() const override { return "Form"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;

private:
    struct Field {
        std::string label;
        FormLabelPos pos;
        Widget* widget;       // raw pointer, owned by m_children
    };
    std::vector<Field> m_fields;
    Widget* m_submitBtn = nullptr;

    Font* m_font = nullptr;
    EdgeInsets m_padding{12, 12, 12, 12};
    float m_spacing = 10;
    float m_labelWidth = 80;

    std::string getFieldValue(Widget* w) const;
};

} // namespace ui

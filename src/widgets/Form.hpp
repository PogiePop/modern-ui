#pragma once
#include <string>
#include "Widget.hpp"
#include "widgets/Container.hpp"
#include "res/Theme.hpp"

namespace ui {

enum class FormLabelPos { Top, Left };

class FormField : public Container {
public:
    FormField(const std::string& label, std::unique_ptr<Widget> input, FormLabelPos pos = FormLabelPos::Top);

    void setLabel(const std::string& t) { m_labelText = t; }
    void setLabelPos(FormLabelPos p);
    void setLabelColorRole(ColorRole r) { m_labelColorRole = r; m_useLabelRole = true; }
    const char* typeName() const override { return "FormField"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    void setFont(class Font* f) { m_font = f; }

private:
    std::string m_labelText;
    FormLabelPos m_labelPos;
    class Font* m_font = nullptr;
    ColorRole m_labelColorRole = ColorRole::TextSecondary;
    bool m_useLabelRole = false;
    static constexpr float LABEL_W = 80;
};

class Form : public Container {
public:
    Form();
    void addField(const std::string& label, std::unique_ptr<Widget> input, FormLabelPos pos = FormLabelPos::Top);
    void setFont(class Font* f);
    void setCompact(bool c) { setSpacing(c ? 4 : 12); setPadding(c ? EdgeInsets{4,4,4,4} : EdgeInsets{8,8,8,8}); }
    const char* typeName() const override { return "Form"; }
private:
    class Font* m_fontPtr = nullptr;
};

} // namespace ui

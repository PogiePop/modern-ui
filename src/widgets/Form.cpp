#include "Form.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"

namespace ui {

FormField::FormField(const std::string& label, std::unique_ptr<Widget> input)
    : Container(LayoutDirection::Vertical), m_labelText(label) {
    setSpacing(4); setPadding({0,0,0,8});
    if (input) input->setMinSize(0, 30);
    addChild(std::move(input));
}

Size FormField::measure(const Size& a) const {
    float lh = m_font ? m_font->lineHeight() : 18;
    float h = lh + 4;
    if (!m_children.empty()) h += m_children[0]->measure({a.width, a.height - lh - 4}).height;
    return {a.width, h};
}

void FormField::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    // Label
    p.drawText({r.x, r.y, m_bounds.width, lh}, m_labelText, Color{0.7f,0.7f,0.8f,1}, TextAlign::Left);
    // Input child
    if (!m_children.empty()) m_children[0]->paint(p);
}

Form::Form() : Container(LayoutDirection::Vertical) { setSpacing(12); setPadding({8,8,8,8}); }

void Form::addField(const std::string& label, std::unique_ptr<Widget> input) {
    auto ff = std::make_unique<FormField>(label, std::move(input));
    ff->setFont(m_fontPtr);
    addChild(std::move(ff));
}

void Form::setFont(Font* f) {
    m_fontPtr = f;
    for (size_t i = 0; i < m_children.size(); ++i) {
        auto* ff = dynamic_cast<FormField*>(m_children[i].get());
        if (ff) ff->setFont(f);
    }
}

} // namespace ui

#include "Form.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"

namespace ui {

FormField::FormField(const std::string& label, std::unique_ptr<Widget> input, FormLabelPos pos)
    : Container(LayoutDirection::Vertical), m_labelText(label), m_labelPos(pos) {
    setLabelPos(pos);
    if (input) { input->setMinSize(0, 30); addChild(std::move(input)); }
}

void FormField::setLabelPos(FormLabelPos p) {
    m_labelPos = p;
    if (p == FormLabelPos::Left) {
        setDirection(LayoutDirection::Horizontal);
        setSpacing(12); setPadding({0,0,0,0});
    } else {
        setDirection(LayoutDirection::Vertical);
        setSpacing(4); setPadding({0,0,0,8});
    }
}

Size FormField::measure(const Size& a) const {
    float lh = m_font ? m_font->lineHeight() : 18;
    if (m_labelPos == FormLabelPos::Left) {
        float inputW = a.width - LABEL_W - spacing();
        float h = lh + 4;
        if (!m_children.empty()) { Size is = m_children[0]->measure({inputW, a.height}); h = std::max(h, is.height); }
        return {a.width, h};
    }
    float h = lh + 4;
    if (!m_children.empty()) h += m_children[0]->measure({a.width, a.height - lh - 4}).height;
    return {a.width, h};
}

void FormField::layout() {
    if (m_labelPos == FormLabelPos::Left && !m_children.empty()) {
        m_children[0]->setBounds({LABEL_W + spacing(), 0, m_bounds.width - LABEL_W - spacing(), m_bounds.height});
        m_children[0]->layout();
    } else if (!m_children.empty()) {
        float lh = m_font ? m_font->lineHeight() : 18;
        m_children[0]->setBounds({0, lh + 4, m_bounds.width, m_bounds.height - lh - 4});
        m_children[0]->layout();
    }
}

void FormField::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    Color labelCol = Color{0.7f,0.7f,0.8f,1};
    if (m_theme) labelCol = m_useLabelRole ? m_theme->color(m_labelColorRole) : m_theme->color(ColorRole::TextSecondary);

    if (m_labelPos == FormLabelPos::Left) {
        p.drawText({r.x, r.y + (m_bounds.height - lh)*0.5f, LABEL_W, lh}, m_labelText, labelCol, TextAlign::Right);
    } else {
        p.drawText({r.x, r.y, m_bounds.width, lh}, m_labelText, labelCol, TextAlign::Left);
    }
    if (!m_children.empty()) m_children[0]->paint(p);
}

Form::Form() : Container(LayoutDirection::Vertical) { setSpacing(12); setPadding({8,8,8,8}); }

void Form::addField(const std::string& label, std::unique_ptr<Widget> input, FormLabelPos pos) {
    auto ff = std::make_unique<FormField>(label, std::move(input), pos);
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

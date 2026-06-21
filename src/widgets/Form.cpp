#include "Form.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"

// Include all input widget types for value extraction
#include "TextInput.hpp"
#include "TextEdit.hpp"
#include "InputNumber.hpp"
#include "Checkbox.hpp"
#include "Switch.hpp"
#include "Dropdown.hpp"
#include "Select.hpp"
#include "Slider.hpp"
#include "DatePicker.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <cstdio>

namespace ui {

Form::Form() { m_focusable = false; }

void Form::addField(const std::string& label, std::unique_ptr<Widget> input,
                    FormLabelPos pos) {
    if (!input) return;
    auto* raw = input.get();
    addChild(std::move(input));
    m_fields.push_back({label, pos, raw});
}

void Form::setSubmitButton(const std::string& text, std::function<void()> onCustom) {
    if (m_submitBtn) { removeChild(m_submitBtn); m_submitBtn = nullptr; }
    auto btn = std::make_unique<Button>(text);
    btn->setVariant(Button::Primary);
    btn->setCornerRadius(6);
    btn->setFont(m_font);
    if (onCustom) {
        btn->setOnClick(std::move(onCustom));
    } else {
        btn->setOnClick([this]() { submit(); });
    }
    m_submitBtn = btn.get();
    addChild(std::move(btn));
}

std::string Form::getFieldValue(Widget* w) const {
    if (!w) return "(null)";

    if (auto* ti = dynamic_cast<TextInput*>(w))  return "\"" + ti->text() + "\"";
    if (auto* te = dynamic_cast<TextEdit*>(w))   return "\"" + te->text() + "\"";
    if (auto* in = dynamic_cast<InputNumber*>(w)) return std::to_string(in->value());
    if (auto* cb = dynamic_cast<Checkbox*>(w))    return cb->checked() ? "true" : "false";
    if (auto* sw = dynamic_cast<Switch*>(w))      return sw->checked() ? "ON" : "OFF";
    if (auto* dd = dynamic_cast<Dropdown*>(w))    return "\"" + dd->selectedText() + "\"";
    if (auto* sl = dynamic_cast<Select*>(w))      return "\"" + sl->selectedText() + "\"";
    if (auto* sd = dynamic_cast<Slider*>(w)) {
        char buf[32]; snprintf(buf, sizeof(buf), "%.1f", sd->value()); return buf;
    }
    if (auto* dp = dynamic_cast<DatePicker*>(w))  return dp->date();

    return "(unknown type)";
}

void Form::submit() {
    printf("\n========== Form Submit ==========\n");
    for (size_t i = 0; i < m_fields.size(); ++i) {
        auto& f = m_fields[i];
        printf("  %s: %s\n", f.label.c_str(), getFieldValue(f.widget).c_str());
    }
    printf("==================================\n\n");
}

Size Form::measure(const Size& a) const {
    float lh = m_font ? m_font->lineHeight() : 18;
    float totalH = m_padding.vertical();
    int visibleCount = 0;

    for (auto& f : m_fields) {
        if (!f.widget || !f.widget->visible()) continue;
        visibleCount++;
        Size s = f.widget->measure({a.width - m_padding.horizontal(), a.height});
        if (f.pos == FormLabelPos::Left) {
            totalH += std::max(lh + 4, s.height);
        } else {
            totalH += lh + 4 + s.height;
        }
    }
    if (m_submitBtn && m_submitBtn->visible()) {
        Size bs = m_submitBtn->measure({a.width - m_padding.horizontal(), 40});
        totalH += bs.height;
        visibleCount++;
    }
    if (visibleCount > 1) totalH += m_spacing * (visibleCount - 1);
    return {a.width, totalH};
}

void Form::layout() {
    float lh = m_font ? m_font->lineHeight() : 18;
    float y = m_padding.top;
    float availW = m_bounds.width - m_padding.horizontal();
    if (availW < 40) availW = 40;

    // Compute label width
    m_labelWidth = 80;
    if (m_font) {
        for (auto& f : m_fields) {
            float tw = m_font->measureText(f.label) + 8;
            if (tw > m_labelWidth) m_labelWidth = tw;
        }
    }

    for (auto& f : m_fields) {
        if (!f.widget || !f.widget->visible()) continue;
        Size ms = f.widget->measure({availW, m_bounds.height - y});

        if (f.pos == FormLabelPos::Left) {
            float inputX = m_labelWidth + 10;
            float inputW = availW - inputX;
            if (inputW < 40) inputW = 40;
            float h = std::max(lh + 4, ms.height);
            f.widget->setBounds({m_padding.left + inputX, y, inputW, h});
            y += h + m_spacing;
        } else {
            f.widget->setBounds({m_padding.left, y + lh + 4, availW, ms.height});
            y += lh + 4 + ms.height + m_spacing;
        }
        f.widget->layout();
    }

    // Submit button
    if (m_submitBtn && m_submitBtn->visible()) {
        Size bs = m_submitBtn->measure({availW, 40});
        float bw = bs.width < 100 ? 100 : bs.width;
        if (bw > availW) bw = availW;
        float bx = m_padding.left + (availW - bw) * 0.5f;
        m_submitBtn->setBounds({bx, y, bw, bs.height});
        m_submitBtn->layout();
    }
}

void Form::paint(Painter& p) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    Color labelCol = Color{0.7f, 0.7f, 0.8f, 1};
    if (m_theme) labelCol = m_theme->color(ColorRole::TextSecondary);

    for (auto& f : m_fields) {
        if (!f.widget || !f.widget->visible()) continue;
        auto& w = f.widget;
        Rect wr = w->screenRect();

        if (f.pos == FormLabelPos::Left) {
            float ly = wr.y + (wr.height - lh) * 0.5f;
            p.drawText({r.x + m_padding.left, ly, m_labelWidth, lh},
                       f.label, labelCol, TextAlign::Right);
        } else {
            p.drawText({r.x + m_padding.left, wr.y - lh - 4, wr.width, lh},
                       f.label, labelCol, TextAlign::Left);
        }
        w->paint(p);
    }
    if (m_submitBtn && m_submitBtn->visible()) m_submitBtn->paint(p);
}

} // namespace ui

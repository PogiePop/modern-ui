#include "MessageBox.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "core/Icon.hpp"
#include "res/Font.hpp"
#include "widgets/Button.hpp"
#include <cstdio>

namespace ui {

MessageBox::MessageBox() { m_visible = false; m_focusable = true; setZIndex(10000); }

void MessageBox::addButton(const std::string& label, std::function<void()> onClick) {
    auto btn = std::make_unique<Button>(label);
    btn->setOnClick([this, cb = std::move(onClick)]() {
        if (cb) cb();
        close();
    });
    btn->setCornerRadius(6);
    btn->setMinSize(80, 30);
    addChild(std::move(btn));
}
void MessageBox::clearButtons() { removeAllChildren(); }

std::vector<std::string> MessageBox::wrapText(const std::string& text, float maxW) const {
    std::vector<std::string> lines;
    if (!m_font) { lines.push_back(text); return lines; }
    std::string cur;
    for (char c : text) {
        if (c == '\n') { lines.push_back(cur); cur.clear(); continue; }
        std::string test = cur + c;
        if (m_font->measureText(test) > maxW && !cur.empty()) {
            lines.push_back(cur); cur.clear(); cur += c;
        } else { cur += c; }
    }
    if (!cur.empty()) lines.push_back(cur);
    if (lines.empty()) lines.push_back("");
    return lines;
}

void MessageBox::show(Widget* parent, float w, float h) {
    m_parentWidget = parent; m_w = w; m_h = h; m_open = true; m_visible = true;
    if (m_children.empty()) addButton("OK", nullptr);
    layout(); // position buttons (overlay skips Container's layout)
    // Adjust height for wrapped text
    if (m_font) {
        auto lines = wrapText(m_message, w - 40);
        float needed = 80 + lines.size() * m_font->lineHeight() + 50;
        if (needed > h) m_h = needed;
    }
    if (parent) {
        float px = (parent->bounds().width - m_w) * 0.5f;
        float py = (parent->bounds().height - m_h) * 0.5f;
        setBounds({px, py, m_w, m_h});
    }
}

void MessageBox::close() { m_open = false; m_visible = false; }

Rect MessageBox::screenRect() const {
    if (m_parentWidget && m_open)
        return {0, 0, m_parentWidget->bounds().width, m_parentWidget->bounds().height};
    Point g = globalPosition();
    return {g.x, g.y, m_w, m_h};
}

void MessageBox::layout() {
    if (m_children.empty()) return;
    float bw = 85, bh = 30, gap = 10;
    float totalW = m_children.size() * bw + (m_children.size()-1) * gap;
    float startX = (m_w - totalW) * 0.5f;
    float btnY = m_h - bh - 16;
    for (size_t i = 0; i < m_children.size(); ++i)
        m_children[i]->setBounds({startX + i*(bw+gap), btnY, bw, bh});
}

void MessageBox::paint(Painter& painter) {
    if (!m_open) return;
    Rect r = screenRect();
    // Frosted backdrop: semi-transparent dark + hint of blur
    painter.drawRect({0, 0, r.width, r.height}, Color{0, 0, 0, 0.55f});
    // Light frosted panel behind dialog
    Rect dlg{(r.width - m_w)*0.5f, (r.height - m_h)*0.5f, m_w, m_h};
    painter.drawRect({dlg.x-4, dlg.y-4, m_w+8, m_h+8}, Color{1,1,1,0.08f});
    // Dialog card
    painter.drawRoundedRect(dlg, Color::fromHex(0xFF2A2A3E), 12);
    painter.drawRectOutline(dlg, Color::fromHex(0xFF555577), 1);

    // Icon
    float iconX = dlg.x + 20, iconY = dlg.y + 20;
    if (m_icon != IconType::None) {
        drawIcon(painter, m_icon, iconX, iconY, 36,
                 m_icon == IconType::Warning ? Color{1,0.8f,0.2f,1} :
                 m_icon == IconType::Error   ? Color{1,0.2f,0.2f,1} :
                 m_icon == IconType::Info    ? Color{0.2f,0.5f,1,1} :
                 Color{0.4f,0.7f,1,1});
    }
    float textX = (m_icon != IconType::None) ? iconX + 48 : dlg.x + 20;
    float textW = m_w - (textX - dlg.x) - 20;

    // Title
    painter.drawText({textX, dlg.y+16, textW, 24}, m_title, Color{1,1,1,1}, TextAlign::Left);
    // Message (wrapped)
    auto lines = wrapText(m_message, textW);
    float lh = m_font ? m_font->lineHeight() : 16;
    float y = dlg.y + 42;
    for (const auto& line : lines) {
        painter.drawText({textX, y, textW, lh}, line, Color{0.8f,0.8f,0.8f,1}, TextAlign::Left);
        y += lh;
    }
    // Paint button children (positioned relative to dialog)
    for (auto& child : m_children)
        if (child->visible()) child->paint(painter);
}

bool MessageBox::onMouseDown(MouseEvent& e) {
    Point g = globalPosition(); // dialog top-left in screen space
    for (auto& child : m_children) {
        Point clp{e.localPos.x - g.x - child->bounds().x, e.localPos.y - g.y - child->bounds().y};
        MouseEvent ce = e; ce.localPos = clp;
        if (child->hitTest(clp) && child->onMouseDown(ce)) return true;
    }
    return true;
}

bool MessageBox::onMouseUp(MouseEvent& e) {
    Point g = globalPosition();
    for (auto& child : m_children) {
        Point clp{e.localPos.x - g.x - child->bounds().x, e.localPos.y - g.y - child->bounds().y};
        MouseEvent ce = e; ce.localPos = clp;
        if (child->onMouseUp(ce)) return true;
    }
    return true;
}
bool MessageBox::onKeyDown(KeyEvent& e) {
    if (e.type == KeyEvent::Down && e.key == 256) { close(); return true; }
    return true;
}
Widget* MessageBox::hitTest(Point) { return m_open ? this : nullptr; }

} // namespace ui

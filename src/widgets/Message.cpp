#include "Message.hpp"
#include "Avatar.hpp"
#include "core/Painter.hpp"
#include "res/Font.hpp"
#include "res/Theme.hpp"
#include "widgets/Container.hpp"
#include <cmath>

namespace ui {

MessageBubble::MessageBubble() { m_focusable = false; }

void MessageBubble::setSender(const std::string& name, const std::string& avInit, const Color& avColor) {
    m_sender = name; m_avatarInit = avInit; m_avatarColor = avColor;
    m_avatar = std::make_unique<Avatar>();
    m_avatar->setInitials(avInit); m_avatar->setColor(avColor); m_avatar->setSize(36);
}

void MessageBubble::setText(const std::string& text) { m_text = text; }

Size MessageBubble::measure(const Size&) const {
    float lh = m_font ? m_font->lineHeight() : 18;
    float tw = m_font ? m_font->measureText(m_text) : m_text.size()*8.0f;
    // Cap at max width, compute wrapped lines
    float actualW = tw < m_maxTextW ? tw : m_maxTextW;
    int lines = tw < m_maxTextW ? 1 : (int)ceilf(tw / m_maxTextW);
    float nameH = m_sender.empty() ? 0 : lh + 2;
    float h = nameH + lines * lh + 20; // name + text lines + padding
    return {m_maxTextW + 56, h};
}

void MessageBubble::layout() {
    if (m_avatar) {
        float ax = (m_side == Left) ? 4 : m_bounds.width - 40;
        m_avatar->setBounds({ax, m_bounds.height - 40, 36, 36});
    }
}

void MessageBubble::paint(Painter& painter) {
    Rect r = screenRect();
    float lh = m_font ? m_font->lineHeight() : 18;
    float tw = m_font ? m_font->measureText(m_text) : m_text.size()*8.0f;
    float actualW = tw < m_maxTextW ? tw : m_maxTextW;
    int lines = tw < m_maxTextW ? 1 : (int)ceilf(tw / m_maxTextW);
    float padX = 10, padY = 8;
    float nameH = m_sender.empty() ? 0 : lh + 2;
    float bubbleW = actualW + padX*2, bubbleH = lines * lh + padY*2;
    float bubbleX = (m_side == Left) ? r.x + 46 : r.x + r.width - 46 - bubbleW;
    float bubbleY = r.y + nameH;

    Color bubbleBg = (m_side == Left) ? Color::fromHex(0xFF2A2A3E) : Color::fromHex(0xFF3B5FEF);
    if (m_theme && m_side == Right) {
        ColorRole cr = m_useColorRole ? m_colorRole : ColorRole::Primary;
        bubbleBg = m_theme->color(cr);
    } else if (m_theme && m_side == Left) {
        bubbleBg = m_theme->color(ColorRole::Surface);
    }
    float rad = (m_variant == Compact) ? 4.0f : ((m_variant == Card) ? 12.0f : 8.0f);
    painter.drawRoundedRect({bubbleX, bubbleY, bubbleW, bubbleH}, bubbleBg, rad);
    if (m_variant == Card) {
        Color borderCol = m_theme ? m_theme->color(ColorRole::Border) : Color{0.3f,0.3f,0.3f,1};
        painter.drawRectOutline({bubbleX, bubbleY, bubbleW, bubbleH}, borderCol, 1);
    }

    // Draw wrapped text lines
    std::string remaining = m_text;
    for (int i = 0; i < lines && !remaining.empty(); ++i) {
        std::string line = remaining;
        if (m_font && m_font->measureText(line) > m_maxTextW) {
            // Find wrap point
            size_t cut = line.size();
            while (cut > 0 && m_font->measureText(line.substr(0, cut)) > m_maxTextW) --cut;
            line = remaining.substr(0, cut);
            remaining = remaining.substr(cut);
        } else remaining.clear();
        painter.drawText({bubbleX+padX, bubbleY+padY+i*lh, actualW, lh}, line, Color{1,1,1,1}, TextAlign::Left);
    }

    // Sender name
    if (!m_sender.empty())
        painter.drawText({bubbleX, r.y, bubbleW, lh}, m_sender, Color{0.6f,0.6f,0.6f,1}, TextAlign::Left);

    // Avatar
    if (m_avatar) { m_avatar->setFont(m_font); m_avatar->paint(painter); }

    // Time
    float timeX = (m_side == Left) ? bubbleX + bubbleW + 4 : bubbleX - 60;
    painter.drawText({timeX, bubbleY + bubbleH - 14, 56, 14}, m_time, Color{0.4f,0.4f,0.4f,1}, TextAlign::Left);
}

std::unique_ptr<Widget> MessageBubble::createChatList(
    Font* font, const std::vector<std::tuple<std::string,std::string,Color,Side,std::string>>& msgs)
{
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    c->setPadding(EdgeInsets::all(8));
    c->setSpacing(4);
    for (const auto& [name, av, col, side, text] : msgs) {
        auto mb = std::make_unique<MessageBubble>();
        mb->setFont(font); mb->setSide(side);
        mb->setSender(name, av, col);
        mb->setText(text); mb->setTime("12:00");
        c->addChild(std::move(mb));
    }
    return c;
}

} // namespace ui

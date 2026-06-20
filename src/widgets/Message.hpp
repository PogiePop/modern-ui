#pragma once
#include <string>
#include <vector>
#include "Widget.hpp"
#include "core/Types.hpp"

namespace ui {

class Avatar;

// Single chat message bubble
class MessageBubble : public Widget {
public:
    enum Side { Left, Right }; // Left = received, Right = sent

    MessageBubble();

    void setSender(const std::string& name, const std::string& avatarInitials,
                   const Color& avatarColor);
    void setText(const std::string& text);
    void setTime(const std::string& time) { m_time = time; }
    void setSide(Side s) { m_side = s; }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "MessageBubble"; }
    Size measure(const Size& available) const override;
    void layout() override;
    void paint(Painter& painter) override;

    // Factory: create a list of message bubbles
    static std::unique_ptr<Widget> createChatList(
        class Font* font,
        const std::vector<std::tuple<std::string,std::string,Color,Side,std::string>>& msgs);

private:
    std::string m_sender, m_text, m_time, m_avatarInit;
    Color m_avatarColor{0.3f,0.5f,0.9f,1};
    Side m_side = Left;
    class Font* m_font = nullptr;
    std::unique_ptr<Avatar> m_avatar;
    float m_maxTextW = 300;
};

} // namespace ui

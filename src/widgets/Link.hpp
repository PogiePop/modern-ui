#pragma once
#include <string>
#include <functional>
#include "Widget.hpp"

namespace ui {

class Link : public Widget {
public:
    explicit Link(const std::string& text = "");

    void setText(const std::string& t) { m_text = t; }
    void setUrl(const std::string& url) { m_url = url; }
    void setFont(class Font* f) { m_font = f; }
    void setOnClick(std::function<void()> cb) { m_onClick = std::move(cb); }

    // Mode: Navigate = open in browser, HttpGet = async GET request
    enum Mode { Navigate, HttpGet };
    void setMode(Mode m) { m_mode = m; }

    const char* typeName() const override { return "Link"; }
    Size measure(const Size&) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseEnter(MouseEvent&) override { m_hovered = true; return true; }
    bool onMouseLeave(MouseEvent&) override { m_hovered = false; return true; }

private:
    std::string m_text, m_url;
    std::function<void()> m_onClick;
    class Font* m_font = nullptr;
    bool m_hovered = false;
    Mode m_mode = Navigate;
};

} // namespace ui

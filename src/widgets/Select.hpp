#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Widget.hpp"

namespace ui {

// Inline select/list picker — shows items in a scrollable list
class Select : public Widget {
public:
    Select();

    void addItem(const std::string& item);
    void removeItem(size_t i);
    void clearItems();
    void setSelectedIndex(size_t i);
    size_t selectedIndex() const { return m_selected; }
    const std::string& selectedText() const;
    void setOnChanged(std::function<void(size_t, const std::string&)> cb) { m_onChanged = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "Select"; }
    Size measure(const Size& a) const override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseMove(MouseEvent& e) override;

private:
    std::vector<std::string> m_items;
    size_t m_selected = 0;
    int m_hovered = -1;
    std::function<void(size_t, const std::string&)> m_onChanged;
    class Font* m_font = nullptr;
    static constexpr float ROW_H = 24;
};

} // namespace ui

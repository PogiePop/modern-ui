#pragma once
#include <functional>
#include <string>
#include <vector>
#include "Container.hpp"

namespace ui {

// Simple date picker with year/month navigation and day grid
class DatePicker : public Container {
public:
    DatePicker();

    // Get/set date as YYYY-MM-DD string
    void setDate(const std::string& iso); // "2026-06-21"
    std::string date() const;

    // Get/set individual components
    void setDate(int year, int month, int day);
    int year() const { return m_year; }
    int month() const { return m_month; }  // 1-12
    int day() const { return m_day; }

    void setOnDateChanged(std::function<void(int year, int month, int day)> cb) { m_onChanged = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }

    const char* typeName() const override { return "DatePicker"; }
    Size measure(const Size& a) const override;
    void layout() override;
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;

private:
    int m_year = 2026, m_month = 6, m_day = 21;
    std::function<void(int, int, int)> m_onChanged;
    class Font* m_font = nullptr;

    static constexpr float HEADER_H = 28;
    static constexpr float CELL_W = 30;
    static constexpr float CELL_H = 24;

    int daysInMonth(int y, int m) const;
    int firstWeekday(int y, int m) const; // 0=Sun
    void fireChanged();
};

} // namespace ui

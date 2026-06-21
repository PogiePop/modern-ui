#pragma once
#include <optional>
#include "Container.hpp"

namespace ui {

// Card: elevated surface with rounded corners, shadow, and padding
class Card : public Container {
public:
    Card();

    void setElevation(float e) { m_elevation = e; }
    float elevation() const { return m_elevation; }

    void setBorderRadius(float r) { m_radius = r; m_useRadius = true; }
    void setPadding(const EdgeInsets& p) { Container::setPadding(p); }

    const char* typeName() const override { return "Card"; }
    void paint(Painter& painter) override;

private:
    float m_elevation = 4.0f;
    float m_radius = 8.0f;
    bool m_useRadius = false;
};

} // namespace ui

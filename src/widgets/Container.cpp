#include "Container.hpp"
#include "Widget.hpp"

#include "core/Painter.hpp"
#include "core/Event.hpp"

#include <algorithm>

namespace ui {

Container::Container(LayoutDirection dir)
    : m_direction(dir) {}

void Container::setDirection(LayoutDirection dir) {
    m_direction = dir;
}

void Container::setBackgroundColor(const Color& color) {
    m_bgColor = color;
}

void Container::clearBackgroundColor() {
    m_bgColor.reset();
}

Color Container::backgroundColor() const {
    return m_bgColor.value_or(Color{0, 0, 0, 0});
}

void Container::setChildSizing(size_t index, Sizing horz, Sizing vert) {
    if (index >= m_childLayout.size()) {
        m_childLayout.resize(index + 1);
    }
    m_childLayout[index] = {horz, vert};
}

// Helper to get child layout info (defined inside namespace for access)
namespace {

using ChildLayoutInfo = Container::ChildLayoutInfo;

const ChildLayoutInfo& getLayout(const std::vector<ChildLayoutInfo>& layouts, size_t index) {
    if (index < layouts.size()) return layouts[index];
    static ChildLayoutInfo defaultLayout;
    return defaultLayout;
}

} // anonymous namespace

Size Container::measure(const Size& available) const {
    float w = 0, h = 0;

    if (m_children.empty()) {
        w = m_padding.horizontal();
        h = m_padding.vertical();
    } else if (m_direction == LayoutDirection::Horizontal) {
        float totalWidth = m_padding.horizontal();
        float maxChildH = 0;
        int visCount = 0;
        for (size_t i = 0; i < m_children.size(); ++i) if (m_children[i]->visible()) visCount++;
        float totalSpacing = m_spacing * (visCount > 0 ? visCount - 1 : 0);

        float availPerChild = available.width - m_padding.horizontal() - totalSpacing;
        if (availPerChild < 0) availPerChild = 0;
        if (visCount == 0) visCount = 1;

        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& child = m_children[i];
            if (!child->visible()) continue;
            Size childAvail{availPerChild / visCount, available.height - m_padding.vertical()};
            Size childSize = child->measure(childAvail);
            if (childSize.width < child->minWidth()) childSize.width = child->minWidth();
            if (childSize.height < child->minHeight()) childSize.height = child->minHeight();
            if (childSize.width > child->maxWidth()) childSize.width = child->maxWidth();
            if (childSize.height > child->maxHeight()) childSize.height = child->maxHeight();
            totalWidth += childSize.width;
            if (childSize.height > maxChildH) maxChildH = childSize.height;
        }
        w = totalWidth + totalSpacing;
        h = maxChildH + m_padding.vertical();
    } else {
        float totalHeight = m_padding.vertical();
        float maxChildW = 0;
        int visCount = 0;
        for (size_t i = 0; i < m_children.size(); ++i) if (m_children[i]->visible()) visCount++;
        float totalSpacing = m_spacing * (visCount > 0 ? visCount - 1 : 0);

        float availPerChild = available.height - m_padding.vertical() - totalSpacing;
        if (availPerChild < 0) availPerChild = 0;
        if (visCount == 0) visCount = 1;

        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& child = m_children[i];
            if (!child->visible()) continue;
            Size childAvail{available.width - m_padding.horizontal(), availPerChild / visCount};
            Size childSize = child->measure(childAvail);
            if (childSize.width < child->minWidth()) childSize.width = child->minWidth();
            if (childSize.height < child->minHeight()) childSize.height = child->minHeight();
            if (childSize.width > child->maxWidth()) childSize.width = child->maxWidth();
            if (childSize.height > child->maxHeight()) childSize.height = child->maxHeight();
            if (childSize.width > maxChildW) maxChildW = childSize.width;
            totalHeight += childSize.height;
        }
        w = maxChildW + m_padding.horizontal();
        h = totalHeight + totalSpacing;
    }

    // Clamp to min/max
    if (w < m_minWidth) w = m_minWidth;
    if (h < m_minHeight) h = m_minHeight;
    if (w > m_maxWidth) w = m_maxWidth;
    if (h > m_maxHeight) h = m_maxHeight;

    return {w, h};
}

void Container::layout() {
    if (m_children.empty()) return;

    // Responsive: scale padding/spacing by width ratio (base: 1280px)
    if (m_responsive && m_bounds.width > 0) {
        float scale = std::clamp(m_bounds.width / 1280.0f, 0.5f, 1.5f);
        m_padding = {m_basePadding.top*scale, m_basePadding.right*scale, m_basePadding.bottom*scale, m_basePadding.left*scale};
        m_spacing = m_baseSpacing * scale;
    }

    float contentX = m_padding.left;
    float contentY = m_padding.top;
    float contentW = m_bounds.width - m_padding.horizontal();
    float contentH = m_bounds.height - m_padding.vertical();
    if (contentW < 0) contentW = 0;
    if (contentH < 0) contentH = 0;

    if (m_direction == LayoutDirection::Horizontal) {
        // First pass: measure intrinsic children
        float totalSpacing = m_spacing * (m_children.size() - 1);
        float usedWidth = totalSpacing;
        float totalFlex = 0;
        std::vector<float> childWidths(m_children.size(), 0);

        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& layout = getLayout(m_childLayout, i);
            if (layout.horz.type == SizingType::Fixed) {
                childWidths[i] = layout.horz.value;
                usedWidth += childWidths[i];
            } else if (layout.horz.type == SizingType::Intrinsic) {
                Size avail{contentW, contentH};
                childWidths[i] = m_children[i]->measure(avail).width;
                usedWidth += childWidths[i];
            } else if (layout.horz.type == SizingType::Fill) {
                totalFlex += layout.horz.value;
            }
        }

        float remaining = contentW - usedWidth;
        if (remaining < 0) remaining = 0;

        // Apply alignment
        float extraGap = 0, edgeGap = 0;
        if (totalFlex == 0 && m_children.size() > 1) {
            switch (m_align) {
            case Align::SpaceBetween: extraGap = remaining / (m_children.size()-1); remaining = 0; break;
            case Align::SpaceAround:  extraGap = remaining / m_children.size(); edgeGap = extraGap * 0.5f; remaining = 0; break;
            case Align::SpaceEvenly:  extraGap = remaining / (m_children.size()+1); edgeGap = extraGap; remaining = 0; break;
            case Align::Center: edgeGap = remaining * 0.5f; remaining = 0; break;
            case Align::End:    edgeGap = remaining; remaining = 0; break;
            default: break;
            }
        }

        float x = contentX + edgeGap;
        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& layout = getLayout(m_childLayout, i);
            float childW = childWidths[i];

            if (layout.horz.type == SizingType::Fill) {
                childW = totalFlex > 0 ? (remaining * layout.horz.value / totalFlex) : 0;
            }

            // Cross-axis sizing
            float childH = contentH;
            if (layout.vert.type == SizingType::Fixed) {
                childH = layout.vert.value;
            } else if (layout.vert.type == SizingType::Intrinsic) {
                Size avail{childW, contentH};
                childH = m_children[i]->measure(avail).height;
                if (childH > contentH) childH = contentH;
            }
            // Fill for cross-axis already set to contentH

            // Clamp to child's min/max
            if (childW < m_children[i]->minWidth()) childW = m_children[i]->minWidth();
            if (childH < m_children[i]->minHeight()) childH = m_children[i]->minHeight();
            if (childW > m_children[i]->maxWidth()) childW = m_children[i]->maxWidth();
            if (childH > m_children[i]->maxHeight()) childH = m_children[i]->maxHeight();

            m_children[i]->setBounds({x, contentY, childW, childH});
            x += childW + m_spacing + extraGap;
        }
    } else {
        // Vertical
        float totalSpacing = m_spacing * (m_children.size() - 1);
        float usedHeight = totalSpacing;
        float totalFlex = 0;
        std::vector<float> childHeights(m_children.size(), 0);

        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& layout = getLayout(m_childLayout, i);
            if (layout.vert.type == SizingType::Fixed) {
                childHeights[i] = layout.vert.value;
                usedHeight += childHeights[i];
            } else if (layout.vert.type == SizingType::Intrinsic) {
                Size avail{contentW, contentH};
                childHeights[i] = m_children[i]->measure(avail).height;
                usedHeight += childHeights[i];
            } else if (layout.vert.type == SizingType::Fill) {
                totalFlex += layout.vert.value;
            }
        }

        float remaining = contentH - usedHeight;
        if (remaining < 0) remaining = 0;

        // Apply alignment
        float extraGap = 0, edgeGap = 0;
        if (totalFlex == 0 && m_children.size() > 1) {
            switch (m_align) {
            case Align::SpaceBetween: extraGap = remaining / (m_children.size()-1); remaining = 0; break;
            case Align::SpaceAround:  extraGap = remaining / m_children.size(); edgeGap = extraGap * 0.5f; remaining = 0; break;
            case Align::SpaceEvenly:  extraGap = remaining / (m_children.size()+1); edgeGap = extraGap; remaining = 0; break;
            case Align::Center: edgeGap = remaining * 0.5f; remaining = 0; break;
            case Align::End:    edgeGap = remaining; remaining = 0; break;
            default: break;
            }
        }

        float y = contentY + edgeGap;
        for (size_t i = 0; i < m_children.size(); ++i) {
            auto& layout = getLayout(m_childLayout, i);
            float childH = childHeights[i];

            if (layout.vert.type == SizingType::Fill) {
                childH = totalFlex > 0 ? (remaining * layout.vert.value / totalFlex) : 0;
            }

            // Cross-axis sizing: stretch to content width by default
            float childW = contentW;
            if (layout.horz.type == SizingType::Fixed) {
                childW = layout.horz.value;
            } else if (layout.horz.type == SizingType::Intrinsic) {
                Size avail{contentW, childH};
                childW = m_children[i]->measure(avail).width;
                if (childW > contentW) childW = contentW;
            }

            // Clamp to child's min/max
            if (childW < m_children[i]->minWidth()) childW = m_children[i]->minWidth();
            if (childH < m_children[i]->minHeight()) childH = m_children[i]->minHeight();
            if (childW > m_children[i]->maxWidth()) childW = m_children[i]->maxWidth();
            if (childH > m_children[i]->maxHeight()) childH = m_children[i]->maxHeight();

            m_children[i]->setBounds({contentX, y, childW, childH});
            y += childH + m_spacing + extraGap;
        }
    }

    // Recursively layout children
    for (auto& child : m_children) {
        child->layout();
    }

    // Track content size and auto-detect overflow
    m_contentSize = measure({m_bounds.width, m_bounds.height});
    if (m_autoScroll) {
        m_scrollable = (m_contentSize.height > m_bounds.height + 2 || m_contentSize.width > m_bounds.width + 2);
    }
}

void Container::paint(Painter& painter) {
    Rect r = screenRect();

    // Paint background
    if (m_bgColor.has_value()) {
        painter.drawRect(r, *m_bgColor);
    }

    // Compute max z-index in each subtree
    std::function<int(Widget*)> maxZ = [&](Widget* w) -> int {
        int mz = w->zIndex();
        for (size_t i = 0; i < w->childCount(); ++i) mz = std::max(mz, maxZ(w->childAt(i)));
        return mz;
    };

    // Build sorted order: subtree max z-index; higher = later (on top)
    std::vector<size_t> order(m_children.size());
    for (size_t i = 0; i < order.size(); ++i) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        int za = maxZ(m_children[a].get()), zb = maxZ(m_children[b].get());
        if (za != zb) return za < zb;
        return a < b;
    });

    // Paint children
    if (m_scrollable) {
        Rect viewport{r.x, r.y, m_bounds.width, m_bounds.height};
        painter.pushClip(viewport);
        for (size_t idx : order) {
            auto& child = m_children[idx];
            if (!child->visible()) continue;
            Rect orig = child->bounds();
            child->setPosition(orig.x - m_scrollX, orig.y - m_scrollY);
            child->paint(painter);
            child->setPosition(orig.x, orig.y);
        }
        painter.popClip();
    } else {
        for (size_t idx : order) {
            auto& child = m_children[idx];
            if (!child->visible()) continue;
            float saveX = child->bounds().x, saveY = child->bounds().y;
            float ox = child->slideOffsetX(), oy = child->slideOffsetY();
            if (ox != 0 || oy != 0) child->setPosition(saveX + ox, saveY + oy);
            if (child->hasShadow() && child->opacity() > 0.01f) {
                Rect sr = child->screenRect();
                Color sc = child->shadowColor(); sc.a *= child->opacity();
                painter.drawShadow(sr, 4, sc, child->shadowOffset(), child->shadowRadius());
            }
            Rect cs = child->screenRect();
            cs.x -= 1; cs.y -= 1; cs.width += 2; cs.height += 2;
            painter.pushClip(cs);
            child->paint(painter);
            painter.popClip();
            child->setPosition(saveX, saveY);
        }
    }

    // Auto scrollbar rendering (browser-style)
    if (scrollable()) {
        float sbW = 8, pad = 2, margin = 2;
        bool hasV = maxScrollY() > 0, hasH = maxScrollX() > 0;
        if (hasV && hasH) { sbW += 2; } // corner overlap
        float corner = hasV && hasH ? sbW + pad : 0;

        if (hasV) {
            float sbH = m_bounds.height - pad*2 - (hasH ? sbW + pad : 0);
            float thumbH = std::max(24.0f, sbH * (m_bounds.height / m_contentSize.height));
            float thumbY = r.y + pad + (sbH - thumbH) * (m_scrollY / std::max(maxScrollY(), 1.0f));
            float sx = r.x + m_bounds.width - sbW - margin;
            painter.drawRoundedRect({sx, r.y+pad, sbW, sbH}, Color{0,0,0,0.08f}, 4);
            Color tc = m_scrollBarDragging ? Color{0.5f,0.5f,0.55f,0.9f} : Color{0.35f,0.35f,0.4f,0.7f};
            painter.drawRoundedRect({sx, thumbY, sbW, thumbH}, tc, 4);
        }
        if (hasH) {
            float sbBW = m_bounds.width - pad*2 - (hasV ? sbW + pad : 0);
            float thumbW = std::max(24.0f, sbBW * (m_bounds.width / m_contentSize.width));
            float thumbX = r.x + pad + (sbBW - thumbW) * (m_scrollX / std::max(maxScrollX(), 1.0f));
            float sy = r.y + m_bounds.height - sbW - margin;
            painter.drawRoundedRect({r.x+pad, sy, sbBW, sbW}, Color{0,0,0,0.08f}, 4);
            painter.drawRoundedRect({thumbX, sy, thumbW, sbW}, Color{0.35f,0.35f,0.4f,0.7f}, 4);
        }
        // Corner
        if (hasV && hasH) {
            float cx = r.x + m_bounds.width - sbW - margin;
            float cy = r.y + m_bounds.height - sbW - margin;
            painter.drawRect({cx, cy, sbW, sbW}, Color{0,0,0,0.05f});
        }
    }
}

bool Container::hasOverflow() const {
    return m_contentSize.width > m_bounds.width || m_contentSize.height > m_bounds.height;
}

bool Container::onMouseDown(MouseEvent& e) {
    if (e.button != 0) return false;
    if (scrollable()) {
        float sbW = 8, pad = 2, margin = 2;
        float msY = maxScrollY(), msX = maxScrollX();
        bool hasH = msX > 0;
        if (msY > 0) {
            float sbH = m_bounds.height - pad*2 - (hasH ? sbW + pad : 0);
            float thumbH = std::max(24.0f, sbH * (m_bounds.height / m_contentSize.height));
            float thumbY = pad + (sbH - thumbH) * (m_scrollY / std::max(msY, 1.0f));
            Rect thumb{m_bounds.width - sbW - margin, thumbY, sbW, thumbH};
            if (thumb.contains(e.localPos)) {
                m_scrollBarDragging = true;
                Point g = globalPosition();
                m_sbDragScreenY = g.y + e.localPos.y;
                m_sbDragVal = m_scrollY;
                m_sbTrackScreenY = g.y + pad;
                m_sbTrackLen = sbH;
                m_sbThumbLen = thumbH;
                m_sbMaxScroll = msY; return true;
            }
        }
    }
    return false;
}

bool Container::onMouseUp(MouseEvent&) { m_scrollBarDragging = false; return false; }

bool Container::onMouseMove(MouseEvent& e) {
    if (!m_scrollBarDragging) return false;
    Point g = globalPosition();
    float curScreenY = g.y + e.localPos.y;
    float dy = curScreenY - m_sbDragScreenY;
    float range = m_sbTrackLen - m_sbThumbLen;
    if (range > 0) m_scrollY = std::clamp(m_sbDragVal + dy * m_sbMaxScroll / range, 0.0f, m_sbMaxScroll);
    return true;
}

float Container::maxScrollX() const {
    return std::max(0.0f, m_contentSize.width - m_bounds.width);
}

float Container::maxScrollY() const {
    return std::max(0.0f, m_contentSize.height - m_bounds.height);
}

Widget* Container::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    // Only reject clicks outside our width; height may be exceeded by popup children
    if (localPoint.x < 0 || localPoint.y < 0 || localPoint.x >= m_bounds.width) {
        return nullptr;
    }
    // If scrollable, adjust for scroll offset
    if (m_scrollable) {
        localPoint.x += m_scrollX;
        localPoint.y += m_scrollY;
    }
    // Compute subtree max z-index
    std::function<int(Widget*)> maxZ = [&](Widget* w) -> int {
        int mz = w->zIndex();
        for (size_t i = 0; i < w->childCount(); ++i) mz = std::max(mz, maxZ(w->childAt(i)));
        return mz;
    };
    // Check children: higher subtree max-z first, same: reverse tree order
    std::vector<size_t> hitOrder(m_children.size());
    for (size_t i = 0; i < hitOrder.size(); ++i) hitOrder[i] = i;
    std::stable_sort(hitOrder.begin(), hitOrder.end(), [&](size_t a, size_t b) {
        int za = maxZ(m_children[a].get()), zb = maxZ(m_children[b].get());
        if (za != zb) return za > zb; // higher subtree max-z = checked first
        return a > b;
    });
    for (size_t idx : hitOrder) {
        Point childLocal = localPoint;
        childLocal.x -= m_children[idx]->bounds().x;
        childLocal.y -= m_children[idx]->bounds().y;
        Widget* hit = m_children[idx]->hitTest(childLocal);
        if (hit) return hit;
    }
    return this;
}

bool Container::onScrollWheel(float, float dy) {
    if (m_scrollable) {
        m_scrollY -= dy * 20.0f; // scroll speed
        if (m_scrollY < 0) m_scrollY = 0;
        float maxY = maxScrollY();
        if (m_scrollY > maxY) m_scrollY = maxY;
        return true;
    }
    return false;
}

} // namespace ui

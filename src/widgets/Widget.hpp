#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

#include "core/Types.hpp"
#include "core/Icon.hpp"

namespace ui {

class Painter;
struct MouseEvent;
struct KeyEvent;
struct CharEvent;
struct FocusEvent;
class Theme;

class Widget {
public:
    Widget();
    virtual ~Widget();

    // ---- Tree ----
    Widget* parent() const { return m_parent; }
    size_t childCount() const { return m_children.size(); }
    Widget* childAt(size_t i) const { return m_children[i].get(); }

    void addChild(std::unique_ptr<Widget> child);
    std::unique_ptr<Widget> removeChild(Widget* child);
    void removeAllChildren();

    // ---- Geometry ----
    const Rect& bounds() const { return m_bounds; }
    void setPosition(float x, float y);
    void setSize(float w, float h);
    void setBounds(const Rect& r);

    Point globalPosition() const;
    virtual Rect screenRect() const { Point g = globalPosition(); return {g.x, g.y, m_bounds.width, m_bounds.height}; }
    virtual float scrollOffsetX() const { return 0; }
    virtual float scrollOffsetY() const { return 0; }

    // Layout invalidation
    void markLayoutDirty() { m_layoutDirty = true; }
    bool isLayoutDirty() const { return m_layoutDirty; }
    void clearLayoutDirty() { m_layoutDirty = false; }

    // ---- Sizing hints ----
    float minWidth() const { return m_minWidth; }
    float minHeight() const { return m_minHeight; }
    float maxWidth() const { return m_maxWidth; }
    float maxHeight() const { return m_maxHeight; }
    void setMinSize(float w, float h) { m_minWidth = w; m_minHeight = h; }
    void setMaxSize(float w, float h) { m_maxWidth = w; m_maxHeight = h; }

    // ---- Visibility ----
    bool visible() const { return m_visible; }
    void setVisible(bool v) { m_visible = v; }

    // ---- Enabled ----
    bool enabled() const { return m_enabled; }
    void setEnabled(bool e) { m_enabled = e; }

    // ---- Focus ----
    bool focusable() const { return m_focusable; }
    void setFocusable(bool f) { m_focusable = f; }
    int tabIndex() const { return m_tabIndex; }
    void setTabIndex(int idx) { m_tabIndex = idx; }
    bool focused() const;

    // ---- Identification (for stylesheets / debugging) ----
    const std::string& id() const { return m_id; }
    void setId(const std::string& id) { m_id = id; }
    const std::string& className() const { return m_className; }
    void setClassName(const std::string& name) { m_className = name; }
    virtual const char* typeName() const { return "Widget"; }

    // ---- Z-Index (layer ordering) ----
    void setZIndex(int z) { m_zIndex = z; }
    int zIndex() const { return m_zIndex; }

    // ---- Icon ----
    void setIcon(IconType icon) { m_icon = icon; }
    IconType icon() const { return m_icon; }

    // ---- Frosted glass ----
    void setFrosted(bool f) { m_frosted = f; }
    bool frosted() const { return m_frosted; }

    // ---- Shadow ----
    void setShadow(Point offset, float radius, const Color& color) {
        m_shadowOffset = offset; m_shadowRadius = radius; m_shadowColor = color; m_hasShadow = true;
    }
    void clearShadow() { m_hasShadow = false; }
    bool hasShadow() const { return m_hasShadow; }
    Point shadowOffset() const { return m_shadowOffset; }
    float shadowRadius() const { return m_shadowRadius; }
    Color shadowColor() const { return m_shadowColor; }

    // ---- Animation ----
    float opacity() const { return m_opacity; }
    void setOpacity(float o) { m_opacity = o; }
    void fadeIn(float duration = 0.3f);
    void fadeOut(float duration = 0.3f);
    void slideIn(Point from, float duration = 0.3f);
    bool isAnimating() const;
    float slideOffsetX() const;
    float slideOffsetY() const;

    // Called each frame by App; override to add custom animations
    virtual void tickAnimation(float dt);

    // ---- Theme ----
    void setTheme(Theme* theme) { m_theme = theme; }
    Theme* themePtr() const { return m_theme; }
    Theme& theme() const;

    // ---- Style overrides (set by stylesheet) ----
    void setStyleColor(const Color& c) { m_styleColor = c; m_hasStyleColor = true; }
    void setStyleBgColor(const Color& c) { m_styleBgColor = c; m_hasStyleBgColor = true; }
    void setStyleFontSize(float s) { m_styleFontSize = s; m_hasStyleFontSize = true; }
    bool hasStyleColor() const { return m_hasStyleColor; }
    Color styleColor() const { return m_styleColor; }
    bool hasStyleBgColor() const { return m_hasStyleBgColor; }
    Color styleBgColor() const { return m_styleBgColor; }
    bool hasStyleFontSize() const { return m_hasStyleFontSize; }
    float styleFontSize() const { return m_styleFontSize; }

    // ---- Layout ----
    virtual Size measure(const Size& available) const = 0;
    virtual void layout();

    // ---- Paint ----
    virtual void paint(Painter& painter) = 0;

    // ---- Hit testing ----
    virtual Widget* hitTest(Point localPoint);

    // ---- Event handlers (return true if handled) ----
    virtual bool onMouseDown(MouseEvent& event);
    virtual bool onMouseUp(MouseEvent& event);
    virtual bool onMouseMove(MouseEvent& event);
    virtual bool onMouseEnter(MouseEvent& event);
    virtual bool onMouseLeave(MouseEvent& event);
    virtual bool onKeyDown(KeyEvent& event);
    virtual bool onKeyUp(KeyEvent& event);
    virtual bool onCharInput(CharEvent& event);
    virtual bool onScrollWheel(float dx, float dy); // scroll event, return true if handled
    virtual void onFocusGained();
    virtual void onFocusLost();

protected:
    Widget* m_parent = nullptr;
    std::vector<std::unique_ptr<Widget>> m_children;
    Rect m_bounds;
    bool m_visible = true;
    bool m_enabled = true;
    bool m_focusable = false;
    int m_tabIndex = 0;
    float m_minWidth = 0, m_minHeight = 0;
    float m_maxWidth = 1e9f, m_maxHeight = 1e9f;
    Theme* m_theme = nullptr;
    std::string m_id;
    std::string m_className;
    bool m_layoutDirty = false;
    bool m_hasShadow = false;
    Point m_shadowOffset{2, 2};
    float m_shadowRadius = 8;
    Color m_shadowColor{0, 0, 0, 0.3f};
    int m_zIndex = 0;
    IconType m_icon = IconType::None;
    bool m_frosted = false;
    // Animation
    float m_opacity = 1.0f;
    float m_opacityTarget = 1.0f;
    float m_opacitySpeed = 0;
    float m_slideFromX = 0, m_slideFromY = 0;
    float m_slideElapsed = 0, m_slideDuration = 0;
    bool m_sliding = false;
    // Style overrides
    Color m_styleColor{0,0,0,0};
    Color m_styleBgColor{0,0,0,0};
    float m_styleFontSize = 0;
    bool m_hasStyleColor = false;
    bool m_hasStyleBgColor = false;
    bool m_hasStyleFontSize = false;
};

} // namespace ui

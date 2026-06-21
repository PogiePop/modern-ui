#pragma once

#include <functional>
#include <vector>
#include <cmath>
#include <algorithm>

#include "core/Types.hpp"

namespace ui {

// ============================================================
// Easing functions (normalized time 0..1 -> 0..1)
// ============================================================
namespace Easing {
    inline float linear(float t) { return t; }
    inline float easeInQuad(float t) { return t * t; }
    inline float easeOutQuad(float t) { return t * (2 - t); }
    inline float easeInOutQuad(float t) { return t < 0.5f ? 2*t*t : -1+(4-2*t)*t; }
    inline float easeInCubic(float t) { return t * t * t; }
    inline float easeOutCubic(float t) { float u = t-1; return u*u*u + 1; }
    inline float easeInOutCubic(float t) {
        return t < 0.5f ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1;
    }
    inline float easeOutBack(float t) {
        float c1=1.70158f; float c3=c1+1;
        return 1+c3*std::pow(t-1,3)+c1*std::pow(t-1,2);
    }
    inline float easeOutBounce(float t) {
        if (t < 1.0f/2.75f) return 7.5625f*t*t;
        if (t < 2.0f/2.75f) { t-=1.5f/2.75f; return 7.5625f*t*t+0.75f; }
        if (t < 2.5f/2.75f) { t-=2.25f/2.75f; return 7.5625f*t*t+0.9375f; }
        t-=2.625f/2.75f; return 7.5625f*t*t+0.984375f;
    }
    inline float easeOutExpo(float t) { return t >= 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
}

using EasingFn = float(*)(float);

// ============================================================
// Transition descriptor (per-property timing)
// ============================================================
struct Transition {
    float duration = 0.3f;
    float delay = 0.0f;
    EasingFn easing = Easing::easeOutCubic;
    bool enabled = true;

    static Transition none() { return {0, 0, Easing::linear, false}; }
    static Transition linear(float dur) { return {dur, 0, Easing::linear, true}; }
    static Transition spring() { return {0.5f, 0, Easing::easeOutBack, true}; }
};

// ============================================================
// AnimatedValue<T> — templated animatable value (EUI-NEO pattern)
// ============================================================
template<typename T>
struct AnimatedValue {
    T current{};
    T target{};
    T start{};
    float elapsed = 0;
    float delay = 0;
    float duration = 0.3f;
    EasingFn easing = Easing::easeOutCubic;
    bool active = false;

    void setTarget(const T& t, const Transition& tr = {}) {
        if (!tr.enabled || tr.duration <= 0) { setImmediate(t); return; }
        if (t == target && active) return;
        start = current;
        target = t;
        elapsed = 0;
        delay = tr.delay;
        duration = tr.duration;
        easing = tr.easing;
        active = true;
    }

    void setImmediate(const T& v) {
        current = target = v;
        start = v;
        active = false;
    }

    // Returns true if just completed
    bool tick(float dt) {
        if (!active) return false;
        if (delay > 0) { delay -= dt; if (delay > 0) return false; elapsed = -delay; delay = 0; }
        elapsed += dt;
        float t = duration > 0 ? elapsed / duration : 1.0f;
        if (t >= 1.0f) { current = target; active = false; return true; }
        float f = easing(t);
        current = lerp(start, target, f);
        return false;
    }

    bool isAnimating() const { return active; }
    const T& value() const { return current; }
    T& value() { return current; }

private:
    // Default lerp for float
    static T lerp(const T& a, const T& b, float f) { return a + (b - a) * f; }
};

// Specialization for Color
template<>
inline Color AnimatedValue<Color>::lerp(const Color& a, const Color& b, float f) {
    return {a.r + (b.r - a.r) * f, a.g + (b.g - a.g) * f,
            a.b + (b.b - a.b) * f, a.a + (b.a - a.a) * f};
}

// Specialization for Point
template<>
inline Point AnimatedValue<Point>::lerp(const Point& a, const Point& b, float f) {
    return {a.x + (b.x - a.x) * f, a.y + (b.y - a.y) * f};
}

// ============================================================
// SmoothedValue<T> — exponential smoothing (EUI-NEO pattern)
// ============================================================
template<typename T>
struct SmoothedValue {
    T current{};
    T target{};
    float speed = 10.0f;

    void setTarget(const T& t) { target = t; }
    void setImmediate(const T& v) { current = target = v; }
    void tick(float dt) {
        float factor = 1.0f - std::exp(-speed * dt);
        current = current + (target - current) * factor;
    }
    const T& value() const { return current; }
};

// ============================================================
// AnimValue — legacy float-only (kept for compatibility)
// ============================================================
struct AnimValue {
    float current = 0;
    float target = 0;
    float start = 0;
    float elapsed = 0;
    float duration = 0.3f;
    EasingFn easing = Easing::easeOutCubic;
    bool active = false;

    void setTarget(float t, float dur = 0.3f, EasingFn e = Easing::easeOutCubic) {
        if (t == current && !active) return;
        start = current;
        target = t;
        elapsed = 0;
        duration = dur > 0 ? dur : 0.001f;
        easing = e;
        active = true;
    }

    void setImmediate(float v) { current = target = v; active = false; }

    bool tick(float dt) {
        if (!active) return false;
        elapsed += dt;
        float t = elapsed / duration;
        if (t >= 1.0f) { current = target; active = false; return true; }
        current = start + (target - start) * easing(t);
        return false;
    }
};

// ============================================================
// AnimationScheduler — higher-level animation runner
// ============================================================
class AnimationScheduler {
public:
    using AnimCallback = std::function<void(float)>;

    void animate(float* value, float target, float duration, EasingFn easing, AnimCallback onUpdate = nullptr);
    void update(float dt);
    bool hasActive() const;
    size_t count() const { return m_animations.size(); }

private:
    struct AnimState {
        float* value = nullptr;
        float start = 0;
        float target = 0;
        float elapsed = 0;
        float duration = 0.3f;
        EasingFn easing = Easing::easeOutCubic;
        AnimCallback onUpdate;
    };
    std::vector<AnimState> m_animations;
};

} // namespace ui

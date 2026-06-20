#pragma once

#include <functional>
#include <vector>
#include <cmath>

#include "core/Types.hpp"

namespace ui {

// Easing functions (normalized time 0..1 -> 0..1)
namespace Easing {
    inline float linear(float t) { return t; }
    inline float easeInQuad(float t) { return t * t; }
    inline float easeOutQuad(float t) { return t * (2 - t); }
    inline float easeInOutQuad(float t) { return t < 0.5f ? 2*t*t : -1+(4-2*t)*t; }
    inline float easeInCubic(float t) { return t * t * t; }
    inline float easeOutCubic(float t) { float u = t-1; return u*u*u + 1; }
    inline float easeOutBack(float t) { float c1=1.70158f; float c3=c1+1; return 1+c3*powf(t-1,3)+c1*powf(t-1,2); }
    inline float easeOutBounce(float t) {
        if (t < 1.0f/2.75f) return 7.5625f*t*t;
        if (t < 2.0f/2.75f) { t-=1.5f/2.75f; return 7.5625f*t*t+0.75f; }
        if (t < 2.5f/2.75f) { t-=2.25f/2.75f; return 7.5625f*t*t+0.9375f; }
        t-=2.625f/2.75f; return 7.5625f*t*t+0.984375f;
    }
}

using EasingFn = float(*)(float);

// Animatable property value
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

    void setImmediate(float v) {
        current = target = v;
        active = false;
    }

    bool tick(float dt) {
        if (!active) return false;
        elapsed += dt;
        float t = elapsed / duration;
        if (t >= 1.0f) { current = target; active = false; return true; }
        current = start + (target - start) * easing(t);
        return false;
    }
};

// Higher-level animation scheduler
class AnimationScheduler {
public:
    using AnimCallback = std::function<void(float)>;

    void animate(float* value, float target, float duration, EasingFn easing, AnimCallback onUpdate = nullptr);
    void update(float dt);
    bool hasActive() const;

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

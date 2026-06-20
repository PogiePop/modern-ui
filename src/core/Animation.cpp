#include "Animation.hpp"

namespace ui {

void AnimationScheduler::animate(float* value, float target, float duration,
                                  EasingFn easing, AnimCallback onUpdate) {
    AnimState state;
    state.value = value;
    state.start = *value;
    state.target = target;
    state.duration = duration > 0 ? duration : 0.001f;
    state.easing = easing;
    state.onUpdate = std::move(onUpdate);
    m_animations.push_back(std::move(state));
}

void AnimationScheduler::update(float dt) {
    for (auto it = m_animations.begin(); it != m_animations.end(); ) {
        it->elapsed += dt;
        float t = it->elapsed / it->duration;
        if (t >= 1.0f) {
            *it->value = it->target;
            if (it->onUpdate) it->onUpdate(it->target);
            it = m_animations.erase(it);
        } else {
            *it->value = it->start + (it->target - it->start) * it->easing(t);
            if (it->onUpdate) it->onUpdate(*it->value);
            ++it;
        }
    }
}

bool AnimationScheduler::hasActive() const {
    return !m_animations.empty();
}

} // namespace ui

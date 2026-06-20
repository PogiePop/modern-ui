#pragma once
#include <functional>
#include <vector>
#include <mutex>

namespace ui {

// Thread-safe queue for posting tasks back to the main thread
class AsyncTaskQueue {
public:
    static AsyncTaskQueue& instance();
    void post(std::function<void()> task);
    void process(); // call from main thread each frame
    bool hasPending() const;

private:
    std::vector<std::function<void()>> m_tasks;
    mutable std::mutex m_mutex;
};

// Run a function on a background thread, then post result to main thread
template<typename T>
void runAsync(std::function<T()> work, std::function<void(T)> onDone) {
    std::thread([work, onDone]() {
        T result = work();
        AsyncTaskQueue::instance().post([onDone, result]() { onDone(result); });
    }).detach();
}

} // namespace ui

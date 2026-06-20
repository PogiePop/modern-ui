#include "AsyncTask.hpp"
#include <algorithm>

namespace ui {

AsyncTaskQueue& AsyncTaskQueue::instance() { static AsyncTaskQueue q; return q; }

void AsyncTaskQueue::post(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_tasks.push_back(std::move(task));
}

void AsyncTaskQueue::process() {
    std::vector<std::function<void()>> pending;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        pending.swap(m_tasks);
    }
    for (auto& t : pending) t();
}

bool AsyncTaskQueue::hasPending() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return !m_tasks.empty();
}

} // namespace ui

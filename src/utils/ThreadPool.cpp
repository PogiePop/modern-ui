#include "ThreadPool.hpp"
#include <cstdio>

namespace ui {

ThreadPool::ThreadPool(size_t numThreads) {
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 1) numThreads = 0; // single core — don't create pool
    }
    if (numThreads > 16) numThreads = 16;

    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back([this]() {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock lock(m_mutex);
                    m_cv.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
                    if (m_stop && m_tasks.empty()) return;
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                task();
            }
        });
    }
    if (numThreads > 0)
        printf("[ThreadPool] started with %zu worker threads\n", numThreads);
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for (auto& w : m_workers) {
        if (w.joinable()) w.join();
    }
}

ThreadPool& ThreadPool::instance() {
    static ThreadPool pool(std::thread::hardware_concurrency());
    return pool;
}

void ThreadPool::init(size_t numThreads) {
    // init() is called to configure; instance() creates lazily
    auto& p = instance();
    if (numThreads > 0) {
        printf("[ThreadPool] configured for %zu threads (actual: %zu)\n",
               numThreads, p.workerCount());
    }
}

} // namespace ui

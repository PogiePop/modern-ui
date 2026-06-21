#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <future>
#include <memory>

namespace ui {

// Simple work-stealing thread pool for parallel rendering/layout tasks
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 0);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submit a task, returns future for result
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result_t<F, Args...>> {
        using returnType = typename std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<returnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<returnType> result = task->get_future();
        {
            std::lock_guard lock(m_mutex);
            if (m_stop) throw std::runtime_error("ThreadPool is stopped");
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_cv.notify_one();
        return result;
    }

    // Run tasks in parallel over a range, blocking until all complete
    template<typename Func>
    void parallelFor(size_t start, size_t end, Func&& func) {
        size_t count = end - start;
        if (count == 0) return;
        size_t numWorkers = std::min(count, m_workers.size());
        if (numWorkers <= 1) {
            for (size_t i = start; i < end; ++i) func(i);
            return;
        }
        std::atomic<size_t> next{start};
        std::vector<std::future<void>> futures;
        for (size_t w = 0; w < numWorkers; ++w) {
            futures.push_back(enqueue([&]() {
                for (;;) {
                    size_t i = next.fetch_add(1);
                    if (i >= end) break;
                    func(i);
                }
            }));
        }
        for (auto& f : futures) f.wait();
    }

    size_t workerCount() const { return m_workers.size(); }
    bool enabled() const { return !m_workers.empty(); }

    static ThreadPool& instance();
    static void init(size_t numThreads = 0);

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stop{false};
};

} // namespace ui

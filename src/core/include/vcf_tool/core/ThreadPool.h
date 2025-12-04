#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <type_traits>
#include <stdexcept>


namespace vcf_tool::core {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t thread_count =
        std::thread::hardware_concurrency());

    ~ThreadPool();

    // Non-copyable, non-movable (can be added later if needed)
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * Submit a callable to the pool.
     *
     * F can be any callable: lambda, functor, function pointer, bind, etc.
     * Args... are its arguments.
     *
     * Returns: std::future<R> where R = std::invoke_result_t<F, Args...>
     */
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

private:
    // Worker function entry point
    void worker_loop(std::stop_token st);

    // Task type erased as void()
    using Task = std::function<void()>;

    // Shared state
    std::mutex mutex_;
    std::condition_variable_any cv_;
    bool stopping_{false};
    std::deque<Task> tasks_;

    // Workers
    std::vector<std::jthread> workers_;
};

// ---------- Template method implementation ----------

template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>>
{
    using R = std::invoke_result_t<F, Args...>;

    // package the callable into a task<R()>
    auto bound_task = std::packaged_task<R()>(
        std::bind(
            std::forward<F>(f),
            std::forward<Args>(args)...
        )
    );

    std::future<R> fut = bound_task.get_future();

    {
        std::scoped_lock lock(mutex_);
        if (stopping_) {
            throw std::runtime_error("submit on stopped ThreadPool");
        }

        // Wrap packaged_task into a void() function
        tasks_.emplace_back(
            [task = std::move(bound_task)]() mutable {
                task();
            }
        );

        // Notify one worker (inside lock for better performance)
        cv_.notify_one();
    }

    return fut;
}

} // namespace vcf_tool::core
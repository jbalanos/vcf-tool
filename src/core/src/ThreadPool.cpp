#include <vcf_tool/core/ThreadPool.h>


namespace vcf_tool::core {

ThreadPool::ThreadPool(std::size_t thread_count)
{
    if (thread_count == 0) {
        thread_count = 1;
    }

    workers_.reserve(thread_count);
    for (std::size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back(
            [this](std::stop_token st) {
                worker_loop(st);
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::scoped_lock lock(mutex_);
        stopping_ = true;
    }

    // Request stop for all workers.
    // std::jthread will call join() in its destructor.
    for (auto& worker : workers_) {
        worker.request_stop();
    }

    // Wake up all workers so they can see stopping_ or stop_token
    cv_.notify_all();
}

void ThreadPool::worker_loop(std::stop_token st)
{
    for (;;) {
        Task task;

        {
            std::unique_lock lock(mutex_);

            // wait until:
            //  - there is a task, or
            //  - pool is stopping, or
            //  - stop has been requested
            cv_.wait(lock, st, [this]() {
                return stopping_ || !tasks_.empty();
            });

            if (st.stop_requested()) {
                // Stop requested: we allow finishing queued tasks
                // or we can choose to break immediately depending on policy.
                // Here, we break only if there are no tasks.
                if (tasks_.empty()) {
                    break;
                }
            }

            if (stopping_ && tasks_.empty()) {
                // Pool is stopping and no more tasks
                break;
            }

            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop_front();
            } else {
                // Spurious wakeup or stop_requested with empty tasks
                continue;
            }
        }

        // Execute outside the lock
        if (task) {
            task();
        }
    }
}

} // namespace vcf_tool::core

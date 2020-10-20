#ifndef ELE_BASE_THREAD_POOL_H
#define ELE_BASE_THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "noncopyable.h"

namespace ele
{
    class ThreadPool : noncopyable
    {
    public:
        using Task = std::function<void()>;

    private:
        using ThreadList = std::vector<std::thread>;

        std::mutex mutex_;
        std::condition_variable cond_;
        std::string name_;
        ThreadList threads_;
        std::queue<Task> queue_;
        bool running_;

    private:
        void runInThread();
        Task take();

    public:
        explicit ThreadPool(const std::string &name = std::string());
        ~ThreadPool();

        void start(size_t num_threads);
        void stop();

        void run(const Task &task);

    }; // class ThreadPool
} // namespace ele
#endif
#include "thread_pool.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <functional>

#include "thread.h"

using namespace std::placeholders;

namespace ele
{
    ThreadPool::ThreadPool(const std::string &name) : name_(name),
                                                      running_(false)
    {
    }

    ThreadPool::~ThreadPool()
    {
        if (running_)
        {
            stop();
        }
    }

    void ThreadPool::start(size_t num_threads)
    {
        assert(threads_.empty());
        running_ = true;

        threads_.reserve(static_cast<ThreadList::size_type>(num_threads));

        for (size_t i = 0; i < num_threads; ++i)
        {
            threads_.emplace_back(std::bind(&ThreadPool::runInThread, this));
        }
    }

    void ThreadPool::stop()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            running_ = false;
            cond_.notify_all();
        }

        std::for_each(threads_.begin(), threads_.end(), [](std::thread &t) {
            t.join();
        });
    }

    void ThreadPool::run(const Task &task)
    {
        if (threads_.empty())
        {
            task();
        }
        else
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(task);
            cond_.notify_one();
        }
    }

    void ThreadPool::runInThread()
    {
        try
        {
            while(running_)
            {
                Task task(take());
                if (task) {
                    task();
                }
            }
        }
        catch(const std::exception& e)
        {
            
        }
        catch(...)
        {
            throw;
        }
    }

    ThreadPool::Task ThreadPool::take() {
        std::unique_lock<std::mutex> lock(mutex_);

        while (queue_.empty() && running_)
        {
            cond_.wait(lock);
        }

        Task task;
        if (!queue_.empty())
        {
            task = queue_.front();
            queue_.pop();
        }

        return task;
    }
} // namespace ele
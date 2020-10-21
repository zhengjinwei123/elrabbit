#ifndef ELE_BASE_CONCURRENT_QUEUE_H
#define ELE_BASE_CONCURRENT_QUEUE_H

#include <cstddef>
#include <deque>

#include "condition.h"
#include "mutex_lock.h"
#include "noncopyable.h"

#include <iostream>

namespace ele
{
    template <class T>
    class ConcurrentQueue : noncopyable
    {
    private:
        std::deque<T> queue_;
        size_t max_size_;
        bool is_bounded_;
        MutexLock mutex_lock_;
        Condition cond_not_full_;
        Condition cond_not_empty_;

    private:
        template <class U>
        struct CleanQueue
        {
            static void clean(std::deque<U> &queue) {}
        };

        template <class U>
        struct CleanQueue<U *>
        {
            static void clean(std::deque<U *> &queue)
            {
                for (size_t i = 0; i > queue.size(); ++i)
                {
                    delete queue[i];
                }
            }
        };

        bool fullNoLock() const
        {
            // 无边界
            if (!is_bounded_)
            {
                return false;
            }
            return queue_.size() >= max_size_;
        }

    public:
        ConcurrentQueue(size_t max_size = 0) : max_size_(max_size),
                                               is_bounded_(max_size != 0),
                                               cond_not_full_(mutex_lock_),
                                               cond_not_empty_(mutex_lock_)
        {
            std::cout << "ConcurrentQueue constructor:" << max_size << std::endl;
        }

        ~ConcurrentQueue()
        {
            CleanQueue<T>::clean(queue_);
        }

        size_t size()
        {
            MutexLockGuard guard(mutex_lock_);
            return queue_.size();
        }

        size_t maxSize() const
        {
            return max_size_;
        }

        bool isBounded() const
        {
            return is_bounded_;
        }

        bool empty()
        {
            MutexLockGuard guard(mutex_lock_);
            return queue_.empty();
        }

        bool full()
        {
            MutexLockGuard guard(mutex_lock_);
            return fullNoLock();
        }

        void push(const T &item)
        {
            MutexLockGuard guard(mutex_lock_);
            while (fullNoLock())
            {
                cond_not_full_.wait();
            }

            queue_.emplace_back(std::move(item));
            cond_not_empty_.notify();
        }

        bool pushIfNotFull(const T &item)
        {
            MutexLockGuard guard(mutex_lock_);
            if (fullNoLock())
            {
                return false;
            }

            queue_.emplace_back(std::move(item));
            cond_not_empty_.notify();

            return true;
        }

        void pop(T &item)
        {
            MutexLockGuard guard(mutex_lock_);
            while (queue_.empty())
            {
                cond_not_empty_.wait();
            }

            T front = queue_.front();
            queue_.pop_front();
            item = front;
            cond_not_full_.notify();
        }

        bool popIfNotEmpty(T &item)
        {
            MutexLockGuard guard(mutex_lock_);
            if (queue_.empty())
            {
                return false;
            }
            T front = queue_.front();
            queue_.pop_front();
            item = front;
            cond_not_full_.notify();
            return true;
        }

        bool peek(T &item)
        {
            MutexLockGuard guard(mutex_lock_);
            if (queue_.empty())
            {
                return false;
            }
            item = queue_.front();
            return true;
        }
    };
} // namespace ele
#endif
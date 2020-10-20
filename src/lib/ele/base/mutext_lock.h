#ifndef ELE_BASE_MUTEX_LOCK_H
#define ELE_BASE_MUTEXT_LOCK_H

#include <cassert>
#include <pthread.h>

#include "noncopyable.h"
#include "thread.h"

namespace ele
{
    class MutexLock : noncopyable
    {
    private:
        pthread_mutex_t mutex_;
        pid_t holder_;

    public:
        MutexLock() : mutex_{}, holder_(0)
        {
            assert(pthread_mutex_init(&mutex_, nullptr) == 0);
        }

        ~MutexLock()
        {
            assert(holder_ == 0);
            assert(pthread_mutexattr_destroy(&mutex_) == 0);
        }

        bool isLockedByThisThread()
        {
            return holder_ == Thread::gettid();
        }

        void assertLocked()
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {
            pthread_mutex_lock(&mutex_);
            holder_ = Thread::gettid();
        }

        void unlock()
        {
            holder_ = 0;
            pthread_mutex_unlock(&mutex_);
        }

        pthread_mutex_t *getPthreadMutexPtr()
        {
            return &mutex_;
        }
    };

    class MutexLockGuard : noncopyable
    {
    private:
        MutexLock &mutex_;

    public:
        explicit MutexLockGuard(MutexLock &mutex) : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }
    };

    // 防止类似误用： MutextLockGuard(mutex_)
    // 临时对象不能长时间持有锁， 一产生对象又马上被销毁
    // 正确写法： MutexLockGuard lock(mutex)
    #define MutextLockGuard(x) error "Missing guard object name"
} // namespace ele
#endif
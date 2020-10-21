#ifndef ELE_BASE_SPIN_LOCK_H
#define ELE_BASE_SPIN_LOCK_H

#include <cassert>
#include <pthread.h>

#include "noncopyable.h"
#include "thread.h"

namespace ele
{
    class SpinLock : noncopyable
    {
    private:
        pthread_spinlock_t spinlock_;
        pid_t holder_;

    public:
        SpinLock() : spinlock_{}, holder_(0)
        {
            assert(pthread_spin_init(&spinlock_, PTHREAD_PROCESS_PRIVATE) == 0);
        }

        ~SpinLock()
        {
            assert(holder_ == 0);
            assert(pthread_spin_destroy(&spinlock_));
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
            pthread_spin_lock(&spinlock_);
            holder_ = Thread::gettid();
        }

        void unlock()
        {
            holder_ = 0;
            pthread_spin_unlock(&spinlock_);
        }

        pthread_spinlock_t *getPthreadSpinLock()
        {
            return &spinlock_;
        }
    }; // class SpinLock

    class SpinLockGuard : noncopyable
    {
    private:
        SpinLock &spinlock_;

    public:
        explicit SpinLockGuard(SpinLock &spinlock) : spinlock_(spinlock)
        {
            spinlock_.lock();
        }

        ~SpinLockGuard()
        {
            spinlock_.unlock();
        }
    }; // class SpinLockGuard

#define SpinLockGuard(x) error "Missing guard object name"
} // namespace ele
#endif
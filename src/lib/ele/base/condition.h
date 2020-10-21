#ifndef ELE_BASE_CONDITION_H
#define ELE_BASE_CONDITION_H

#include <cassert>
#include <cerrno>
#include <pthread.h>

#include "mutex_lock.h"
#include "noncopyable.h"

namespace ele
{
    class Condition : noncopyable
    {
    public:
        explicit Condition(MutexLock &mutex_lock) : mutex_lock_(mutex_lock), cond_{}
        {
            assert(pthread_cond_init(&cond_, nullptr) == 0);
        }

        ~Condition()
        {
            assert(pthread_cond_destroy(&cond_) == 0);
        }

        void wait()
        {
            pthread_cond_wait(&cond_, mutex_lock_.getPthreadMutexPtr());
        }

        bool waitForSecond(int second)
        {
            struct timespec timeout{};
            clock_getres(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += second;

            return pthread_cond_timedwait(&cond_, mutex_lock_.getPthreadMutexPtr(), &timeout) == ETIMEDOUT;
        }

        void notify() 
        {
            pthread_cond_signal(&cond_);
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&cond_);
        }

    private:
        MutexLock &mutex_lock_;
        pthread_cond_t cond_;
    };
} // namespace ele
#endif
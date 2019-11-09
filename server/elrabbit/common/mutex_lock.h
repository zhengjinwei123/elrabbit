#ifndef ELRABBIT_COMMON_MUTEX_H
#define ELRABBIT_COMMON_MUTEX_H

#include <memory>
#include <pthread.h>
#include "elrabbit/common/class_util.h"

namespace elrabbit {

namespace common {

class MutexLock: Noncopyable
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

class MutexLockGuard : Noncopyable
{
public:
    explicit MutexLockGuard(MutexLock &m) :
        mutex_lock_(m)
    {
        mutex_lock_.lock();
    }
    ~MutexLockGuard() { mutex_lock_.unlock(); }

private:
    MutexLock& mutex_lock_;
};

}// namespace common

} // namespace elrabbit
#endif // ELRABBIT_COMMON_MUTEX_H
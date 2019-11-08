#ifndef ELRABBIT_COMMON_MUTEX_H
#define ELRABBIT_COMMON_MUTEX_H

#include <memory>
#include <pthread.h>
#include "elrabbit/common/class_util.h"

namespace elrabbit {

namespace common {

class Mutex: Noncopyable
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

private:
    pthread_mutex_t mutex_;
};

class LockGuard : Noncopyable
{
public:
    explicit LockGuard(Mutex &m) :
        mutex_(m)
    {
        mutex_.lock();
    }
    ~LockGuard() { mutex_.unlock(); }

private:
    Mutex& mutex_;
};

}// namespace common

} // namespace elrabbit
#endif // ELRABBIT_COMMON_MUTEX_H
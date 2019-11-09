#ifndef ELRABBIT_COMMON_CONDITION_H
#define ELRABBIT_COMMON_CONDITION_H

#include <pthread.h>
#include "elrabbit/common/class_util.h"
#include "elrabbit/common/mutex_lock.h"
#include "elrabbit/common/exception.h"

namespace elrabbit {

namespace common {

class Condition : Noncopyable
{
public:
    explicit Condition(MutexLock& mutex_lock);

    ~Condition()
    {
        ::pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        if (::pthread_cond_wait(&pcond_, mutex_lock_.getPthreadMutex()) != 0) {
            throw SystemErrorException(
                "Condition wait failed in pthread_cond_wait"
            );
        }
    }

    bool waitForSeconds(double seconds);

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_lock_;
    pthread_cond_t pcond_;
};

} // namespace commmon
} // namespace elrabbit

#endif // ELRABBIT_COMMON_CONDITION_H
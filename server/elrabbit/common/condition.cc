#include "elrabbit/common/condition.h"
#include <errno.h>
#include <stdint.h>

namespace elrabbit {

namespace common {

    Condition::Condition(MutexLock& mutex_lock)
        : mutex_lock_(mutex_lock)
    {
        if (::pthread_cond_init(&pcond_, nullptr) != 0) {
            throw SystemErrorException(
                "Condition constructor failed in pthread_cond_init"
            );
        }
    }

    bool Condition::waitForSeconds(double seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);

        const int64_t nano_seconds_per_second = 1000000000;
        int64_t nanoseconds = static_cast<int64_t>(seconds * nano_seconds_per_second);

        abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / nano_seconds_per_second);
        abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % nano_seconds_per_second);

        int ret = ::pthread_cond_timedwait(&pcond_, mutex_lock_.getPthreadMutex(), &abstime);
        if (ETIMEDOUT == ret) {
            return false;
        } else if (ret != 0) {
            throw SystemErrorException(
                "Condition waitForSeconds failed in pthread_cond_timedwait"
            );
        }

        return true;
    }
} // namespace common
} // namespace elrabbit
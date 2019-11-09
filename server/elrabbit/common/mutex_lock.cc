#include "elrabbit/common/mutex_lock.h"
#include "elrabbit/common/exception.h"

namespace elrabbit {

namespace common {

MutexLock::MutexLock()
{
    if (::pthread_mutex_init(&mutex_, nullptr) != 0) {
        throw SystemErrorException(
            "Mutex constructor failed in pthread_mutex_init"
        );
    }
}

MutexLock::~MutexLock()
{
    ::pthread_mutex_destroy(&mutex_);
}

void MutexLock::lock()
{
    if (::pthread_mutex_lock(&mutex_) != 0) {
        throw SystemErrorException(
            "Mutex lock failed in pthread_mutex_lock"
        );
    }
}

void MutexLock::unlock()
{
    if (::pthread_mutex_unlock(&mutex_) != 0) {
        throw SystemErrorException(
            "Mutex unlock failed in pthread_mutex_unlock"
        );
    }
}

} // namespace common
} // namespace elrabbit


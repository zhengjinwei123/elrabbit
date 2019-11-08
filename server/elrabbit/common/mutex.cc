#include "elrabbit/common/mutex.h"
#include "elrabbit/common/exception.h"

namespace elrabbit {

namespace common {

Mutex::Mutex()
{
    if (::pthread_mutex_init(&mutex_, nullptr) != 0) {
        throw SystemErrorException(
            "Mutex constructor failed in pthread_mutex_init"
        );
    }
}

Mutex::~Mutex()
{
    ::pthread_mutex_destroy(&mutex_);
}

void Mutex::lock()
{
    if (::pthread_mutex_lock(&mutex_) != 0) {
        throw SystemErrorException(
            "Mutex lock failed in pthread_mutex_lock"
        );
    }
}

void Mutex::unlock()
{
    if (::pthread_mutex_unlock(&mutex_) != 0) {
        throw SystemErrorException(
            "Mutex unlock failed in pthread_mutex_unlock"
        );
    }
}

} // namespace common
} // namespace elrabbit


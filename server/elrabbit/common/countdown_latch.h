#ifndef ELRABBIT_COMMON_COUNTDOWN_LATCH_H
#define ELRABBIT_COMMON_COUNTDOWN_LATCH_H

#include "elrabbit/common/class_util.h"
#include "elrabbit/common/mutex_lock.h"
#include "elrabbit/common/condition.h"

namespace elrabbit {

namespace common {

class CountDownLatch : Noncopyable {
public:
    explicit CountDownLatch(int count);
    ~CountDownLatch() {}

    void wait();
    void countDown();

    int getCount() const;

private:
    mutable MutexLock mutex_lock_;
    Condition condition_;
    int count_;
};

} // namespace common
}// namespace elrabbit
#endif // ELRABBIT_COMMON_COUNTDOWN_LATCH_H
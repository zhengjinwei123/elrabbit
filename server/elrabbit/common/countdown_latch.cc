#include "elrabbit/common/countdown_latch.h"

namespace elrabbit {

namespace common {

CountDownLatch::CountDownLatch(int count) :
    mutex_lock_(),
    condition_(mutex_lock_),
    count_(count)
{

}

void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_lock_);
    while(count_ > 0) {
        condition_.wait();
    }
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mutex_lock_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}

int CountDownLatch::getCount() const
{
    MutexLockGuard lock(mutex_lock_);
    return count_;
}


} // namespace elrabbit
}// namespace elrabbit


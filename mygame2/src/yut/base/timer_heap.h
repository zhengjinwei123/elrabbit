#ifndef YUT_BASE_TIMER_HEAP_H
#define YUT_BASE_TIMER_HEAP_H

#include <stdint.h>
#include <yut/base/class_util.h>
#include <functional>
#include <memory>

namespace yut {
namespace base {

class Timestamp;

class TimerHeap {
public:
    using TimerId = int64_t;
    using TimerCallback = std::function<void (TimerId)>;

    TimerHeap();
    ~TimerHeap();

    int64_t getNextTimeoutMillisecond(const Timestamp &now) const;
    TimerId addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_timers = -1);
    void removeTimer(TimerId timer_id);
    void checkTimeout(const Timestamp &now);

private:
    YUT_NONCOPYABLE(TimerHeap)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace base
} // namespace yut
#endif
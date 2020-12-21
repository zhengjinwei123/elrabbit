#include <yut/base/timer_heap.h>

#include <cstddef>
#include <algorithm>
#include <vector>
#include <ext/hash_map>

#include <yut/base/base_logger.h>
#include <yut/base/timestamp.h>

#include <yut/base/min_heap.h>
#include <yut/base/base_heap_value.h>

namespace yut {
namespace base {

namespace timer_heap_impl {

class TimerIdAllocator {
public:
    TimerIdAllocator() : value_(0) {}
    ~TimerIdAllocator() {}

    int64_t getId()
    {
        if (++value_ < 0) {
            value_ = 0;
        }
        return value_;
    }

private:
    int64_t value_;
};

/////////////////////////////////////////////////////////////

class Timer : public BaseHeapValue<Timestamp> {
public:
    using TimerId = TimerHeap::TimerId;
    using TimerCallback = TimerHeap::TimerCallback;

    Timer(TimerId id, const Timestamp &timestamp, int64_t timeout, const TimerCallback &timer_cb, int call_times);
    ~Timer() {}

    TimerId getId() const { return id_; }
    int getTimeout() const { return timeout_; }
    int getCallTimes() const { return call_times_; }
    void decCallTimes() { --call_times_; }
    TimerCallback getCallBack() const { return timer_cb_; }

private:
    TimerId id_;
    int64_t timeout_;
    TimerCallback timer_cb_;
    int call_times_;
};

////////////////////////////////////////////////////////////////
Timer::Timer(TimerId id, const Timestamp &timestamp, int64_t timeout, const TimerCallback &timer_cb, int call_times) :
    BaseHeapValue(timestamp),
    id_(id), timeout_(timeout), timer_cb_(timer_cb), call_times_(call_times)
{
    
}

} // timer_heap_impl
using namespace timer_heap_impl;

///////////////////////////////////////////////////////////////////
class TimerHeap::Impl {
public:
    using TimerId = TimerHeap::TimerId;
    using TimerCallback = TimerHeap::TimerCallback;
    using TimerMap = __gnu_cxx::hash_map<TimerId, Timer *>;

    Impl();
    ~Impl();

    int64_t getNextTimeoutMillisecond(const Timestamp &now) const;
    TimerId addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
    void removeTimer(TimerId timer_id);
    void checkTimeout(const Timestamp &now);

private:
    TimerIdAllocator timer_id_allocator_;
    MinHeap<Timestamp> timer_min_heap_;
    TimerMap timers_;
};

//////////////////////////////////////////////////////////////////////
TimerHeap::Impl::Impl() :
    timer_min_heap_()
{

}

TimerHeap::Impl::~Impl()
{
    for (auto iter = timers_.begin(); iter != timers_.end(); ++iter) {
        delete iter->second;
    }
}

int64_t TimerHeap::Impl::getNextTimeoutMillisecond(const Timestamp &now) const
{
    if (timer_min_heap_.empty()) {
        return -1;
    }

    Timer *timer = dynamic_cast<Timer *>(timer_min_heap_.top());
    if (nullptr == timer) {
        return -1;
    }

    if (timer->getValue() < now) {
        return 0;
    }

    return now.distanceMillisecond(timer->getValue());
}

TimerHeap::Impl::TimerId TimerHeap::Impl::addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
{
    TimerId timer_id = timer_id_allocator_.getId();
    timeout_ms = std::max((int64_t)0, timeout_ms);

    std::unique_ptr<Timer> timer(new Timer(timer_id,
                                        now + timeout_ms,
                                        timeout_ms,
                                        timer_cb,
                                        call_times));
    if (timers_.find(timer_id) != timers_.end()) {
        BASE_ERROR("timer(%lu) already in timer map", timer_id);
        return -1;
    }

    timer_min_heap_.reserve();

    timers_.insert(std::make_pair(timer_id, timer.get()));
    Timer *timer_raw = timer.release();

    timer_min_heap_.insert(timer_raw);

    return timer_id;
}

void TimerHeap::Impl::removeTimer(TimerId timer_id)
{
    auto iter = timers_.find(timer_id);
    if (timers_.end() == iter) {
        return;
    }

    Timer *timer = iter->second;

    timer_min_heap_.erase(timer);

    timers_.erase(iter);

    delete timer;
}

void TimerHeap::Impl::checkTimeout(const Timestamp &now)
{
    for (;;)
    {
        if (timer_min_heap_.empty()) {
            return;
        }

        Timer *timer = dynamic_cast<Timer *>(timer_min_heap_.top());
        if (nullptr == timer) {
            return;
        }

        if (now.millisecondLess(timer->getValue())) {
            return;
        }

        TimerId timer_id = timer->getId();
        TimerCallback timer_cb = timer->getCallBack();

        timer_min_heap_.erase(timer);

        if (timer->getCallTimes() == 1) {
            timers_.erase(timer_id);
            delete timer;
        } else {
            if (timer->getCallTimes() > 0) {
                timer->decCallTimes();
            }

            timer->getValue() += timer->getTimeout();
            
            timer_min_heap_.insert(timer);
        }

        timer_cb(timer_id);
    }
}

///////////////////////////////////////////////////////////
TimerHeap::TimerHeap() :
    pimpl_(new Impl())
{

}

TimerHeap::~TimerHeap()
{

}

int64_t TimerHeap::getNextTimeoutMillisecond(const Timestamp &now) const
{
    return pimpl_->getNextTimeoutMillisecond(now);
}

TimerHeap::TimerId TimerHeap::addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
{
    return pimpl_->addTimer(now, timeout_ms, timer_cb, call_times);
}

void TimerHeap::removeTimer(TimerId timer_id)
{
    pimpl_->removeTimer(timer_id);
}

void TimerHeap::checkTimeout(const Timestamp &now)
{
    pimpl_->checkTimeout(now);
}

} // namespace base
} // namespace yut
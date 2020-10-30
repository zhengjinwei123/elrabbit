#ifndef ELE_BASE_TIMER_HEAP_H
#define ELE_BASE_TIMER_HEAP_H

#include "class_util.h"

#include <stdint.h>
#include <functional>
#include <memory>

namespace ele
{
    class Timestamp;

    class TimerHeap {
        public:
            typedef int64_t TimerId;
            using TimerCallback = std::function<void (TimerId)>;

            TimerHeap();
            ~TimerHeap();

            int64_t getNextTimeoutMillisecond(const Timestamp &now) const;
            TimerId addTimer(const Timestamp &now, int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
            void removeTimer(TimerId timer_id);
            void checkTimeout(const Timestamp &now);
        
        private:
            ELE_NONCOPYABLE(TimerHeap);

            class Impl;
            std::unique_ptr<Impl> pimpl_;
    }; // class TimerHeap
}

#endif
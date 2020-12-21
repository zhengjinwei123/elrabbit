#ifndef YUT_NET_LOOP_SERVICE_H
#define YUT_NET_LOOP_SERVICE_H

#include <yut/base/class_util.h>
#include <stdint.h>
#include <functional>
#include <memory>



namespace yut {
namespace base {

class IOChannel;

class LoopService {
public:
    using TimerId = int64_t;
    using TimerCallback = std::function<void (TimerId)>;

    LoopService();
    ~LoopService();

    void loop();
    void quit();

    TimerId startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1);
    void stopTimer(TimerId timer_id);

private:
    friend class IOChannel;
    bool addIOChannel(IOChannel *io_channel);
    bool removeIOChannel(IOChannel *io_channel);
    bool updateIOChannel(IOChannel *io_channel);

private:
    YUT_NONCOPYABLE(LoopService)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace base
} // namespace yut



#endif
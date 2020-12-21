#ifndef YUT_NET_SIGNAL_QUEUE_H
#define YUT_NET_SIGNAL_QUEUE_H

#include <yut/base/class_util.h>
#include <functional>
#include <memory>

namespace yut {
namespace net {

class LoopService;

class SignalQueue {
public:
    using RecvSignalCallback = std::function<void (int)>;

    SignalQueue(LoopService &loop_service);
    ~SignalQueue();

    LoopService *getLoopService() const;
    void setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb);
    void push(int signum);

private:
    YUT_NONCOPYABLE(SignalQueue)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace net
} // namespace yut
#endif
#include <yut/net/signal_queue.h>

#include <yut/net/loop_service.h>
#include <yut/net/self_pipe.h>
#include <functional>

#include <exception>

namespace yut {
namespace net {

class SignalQueue::Impl {
public:
    using RecvSignalCallback = SignalQueue::RecvSignalCallback;

    Impl(LoopService &loop_service);
    ~Impl();

    LoopService *getLoopService() const;
    void setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb);
    void push(int signum);

private:
    void pipeReadCallback(IOChannel *io_channel);

private:
    LoopService *loop_service_;
    SelfPipe pipe_;
    RecvSignalCallback recv_signal_cb_;
};

///////////////////////////////////////////////////
SignalQueue::Impl::Impl(LoopService &loop_service) :
    loop_service_(&loop_service)
{
    if (pipe_.open()) == false ||
        pipe_.setNonblock() == false ||
        pipe_.setCloseOnExec()) {
        throw std::exception("create signal queue failed in self queue init");  
    }

    pipe_.setRecvSignalCallback(std::bind(&SignalQueue::pipeReadCallback, this, std::placeholders::_1));
    pipe_.attachLoopService(*loop_service_);
}

SignalQueue::Impl::~Impl()
{

}

LoopService *SignalQueue::Impl::getLoopService() const
{
    return loop_service_;
}

void SignalQueue::Impl::setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb)
{
    recv_signal_cb_ = recv_signal_cb;
}

void SignalQueue::Impl::push(int signum)
{
    pipe_.write((const char *)&signum, sizeof(signum));
}

void SignalQueue::Impl::pipeReadCallback(IOChannel *io_channel)
{
    int signum = 0;

    while (pipe_.read(char *)&signum, sizeof(int) > 0) {
        if (recv_signal_cb_) {
            recv_signal_cb_(signum);
        }
    }
}

///////////////////////////////////////////////////
SignalQueue::SignalQueue(LoopService &loop_service) :
    pimpl_(new Impl())
{

}

SignalQueue::~SignalQueue()
{

}

LoopService *SignalQueue::getLoopService() const
{
    return pimpl_->getLoopService();
}

void SignalQueue::setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb)
{
    pimpl_->setRecvSignalCallback(recv_signal_cb);
}

void SignalQueue::push(int signum)
{
    pimpl_->push(signum);
}

} // namespace net
} // namespace yut


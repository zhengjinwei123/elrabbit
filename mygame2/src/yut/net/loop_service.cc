#include <yut/net/loop_service.h>

#include <stdint.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/epoll.h>
#include <cerrno>
#include <cstring>
#include <vector>
#include <ext/hash_set>

#include <yut/base/base_logger.h>
#include <exception>
#include <yut/net/io_channel.h>
#include <yut/base/timer_heap.h>
#include <yut/base/timestamp.h>

#define MAX_EPOLL_TIMEOUT_MSEC (35 * 60 * 1000) // 35 分钟

namespace yut {

using base::TimerHeap;
using base::Timestamp;

namespace net {

class LoopService::Impl {
public:
    using TimerId = LoopService::TimerId;
    using TimerCallback = LoopService::TimerCallback;
    using EventVector = std::vector<struct epoll_event>;
    using IOChannelSet = __gnu_cxx::hash_set<intptr_t>;

    Impl();
    ~Impl();

    bool addIOChannel(IOChannel *io_channel);
    bool removeIOChannel(IOChannel *io_channel);
    bool updateIOChannel(IOChannel *io_channel);

    void loop();
    void quit();

    TimerId startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times);
    void stopTimer(TimerId timer_id);

public:
    bool checkIOChannelExists(IOChannel *io_channel) const;

private:
    bool quit_;
    int epoll_fd_;
    EventVector events_;
    IOChannelSet removed_io_channels_;
    TimerHeap timer_heap_;
};

////////////////////////////////////////////////////////////////////
LoopService::Impl::Impl() :
    quit_(false), epoll_fd_(-1), events_(32)
{
    epoll_fd_ = epoll_create(100000);
    if (-1 == epoll_fd_) {
        throw std::exception("epoll_create failed");
    }
    int flags = ::fcntl(epoll_fd_, F_GETFD, 0);
    if (::fcntl(epoll_fd_, F_SETFD, flags | FD_CLOEXEC) != 0) {
        throw std::exception("fcntl set cloexec failed");
    }
}

LoopService::Impl::~Impl()
{
    if (epoll_fd_ != -1) {
        ::close(epoll_fd_);
        epoll_fd_ = -1;
    }
}

LoopService::Impl::addIOChannel(IOChannel *io_channel)
{
    struct epoll_event event;
    ::memset(&event, 0, sizeof(event));
    event.events = 0;
    event.data.ptr = io_channel;

    if (io_channel->getReadCallback()) {
        event.events |= EPOLLIN | EPOLLPRI;
    }
    if (io_channel->getWriteCallback()) {
        event.events |= EPOLLOUT;
    }

    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, io_channel->getDescriptor(), &event) != 0) {
        BASE_ERROR("epoll_ctl add (%d) failed: %s", io_channel->getDescriptor(), ::strerror(errno));
        return false;
    }

    return true;
}


bool LoopService::Impl::removeIOChannel(IOChannel *io_channel)
{
    struct epoll_event event;
    ::memset(&event, 0, sizeof(event));

    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, io_channel->getDescriptor(), &event) != 0) {
        BASE_ERROR("epoll_ctl del (%d) failed : %s", io_channel->getDescriptor(), ::strerror(errno));
        return false;
    }

    removed_io_channels_.insert((intptr_t)io_channel);
}

bool LoopService::Impl::updateIOChannel(IOChannel *io_channel)
{
    struct epoll_event event;
    ::memset(&event, 0, sizeof(event));
    event.events = 0;
    event.data.ptr = io_channel;

    if (io_channel->getReadCallback()) {
        event.events |= EPOLLIN | EPOLLPRI;
    }
    if (io_channel->getWriteCallback()) {
        event.events |= EPOLLOUT;
    }

    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, io_channel->getDescriptor(), &event) != 0) {
        BASE_ERROR("epoll_ctl mod (%d) failed: %s", io_channel->getDescriptor(), ::strerror(errno));
        return false;
    }

    return true;
}


bool LoopService::Impl::checkIOChannelExists(IOChannel *io_channel) const
{
    return removed_io_channels_.find((intptr_t)io_channel) == removed_io_channels_.end();
}

void LoopService::Impl::loop()
{
    quit_ = false;
    Timestamp now;
    while (!quit_) {
        now.setNow();

        int timer_timeout = timer_heap_.getNextTimeoutMillisecond(now);
        int event_count = ::epoll_wait(epoll_fd_, &events_[0], events_.size(), std::min(timer_timeout, MAX_EPOLL_TIMEOUT_MSEC));
        if (-1 == event_count) {
            if (EINTR == errno) {
                continue;
            } else {
                BASE_ERROR("epoll_wait failed : %s", ::strerror(errno));
                break;
            }
        }


        for (int i = 0; i < event_count; ++i) {
            struct epoll_event *event = &events_[i];
            IOChannel *io_channel = (IOChannel *)event->data.ptr;

            if (event->events & EPOLLOUT) {
                if (checkIOChannelExists(io_channel) == false) {
                    continue;
                }
                (io_channel->getWriteCallback())(io_channel);
            }

#ifdef EPOLLRDHUP
            if (event->events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
#else
            if (event->events & (EPOLLIN | EPOLLPRI)) {
#endif          
                if  (checkIOChannelExists(io_channel) == false) {
                    continue;
                }
                (io_channel->getReadCallback())(io_channel);
            }

            if (event->events & (EPOLLERR | EPOLLHUP)) {
                if (checkIOChannelExists(io_channel) == false) {
                    continue;
                }
                (io_channel->getErrorCallback())(io_channel);
            }
        }

        // timer event call
        now.setNow();
        timer_heap_.checkTimeout(now);

        removed_io_channels_.clear();

        if (event_count >= (int)events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
}

void LoopService::Impl::quit()
{
    quit_ = true;
}

LoopService::Impl::TimerId LoopService::Impl::startTimer(int64_t timeout_ms,  const TimerCallback &timer_cb, int call_times)
{
    Timestamp now;
    now.setNow();

    return timer_heap_.addTimer(now, timeout_ms, timer_cb, call_times);
}

void LoopService::Impl::stopTimer(TimerId timer_id)
{
    timer_heap_.removeTimer(timer_id);
}

////////////////////////////////////////////////////////////////
LoopService::LoopService() :
    pimpl_(new Impl())
{

}

LoopService::~LoopService()
{

}

bool LoopService::addIOChannel(IOChannel *io_channel)
{
    return pimpl_->addIOChannel(io_channel);
}

bool LoopService::removeIOChannel(IOChannel *io_channel)
{
    return pimpl_->removeIOChannel(io_channel);
}

bool LoopService::updateIOChannel(IOChannel *io_channel)
{
    return pimpl_->updateIOChannel(io_channel);
}

void LoopService::loop()
{
    pimpl_->loop();
}
void LoopService::quit()
{
    pimpl_->quit();
}

LoopService::TimerId LoopService::startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = -1)
{
    return pimpl_->startTimer(timeout_ms, timer_cb, call_timescall_times);
}

void LoopService::stopTimer(TimerId timer_id)
{
    pimpl_->stopTimer(timer_id);
}


} // namespace net
} // namespace yut





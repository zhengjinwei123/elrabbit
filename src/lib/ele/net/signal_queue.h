#ifndef ELE_NET_SIGNAL_QUEUE_H
#define ELE_NET_SIGNAL_QUEUE_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>

namespace ele
{
    namespace net
    {
        class IOService;

        class SignalQueue
        {
        public:
            using RecvSignalCallback = std::function<void(int)>;

            SignalQueue(IOService &io_service);
            ~SignalQueue();

            IOService *getIOService() const;
            void setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb);
            void push(int signum);

        private:
            ELE_NONCOPYABLE(SignalQueue);

            class Impl;
            std::unique_ptr<Impl> pimpl_;
        }; // class SignalQueue
    }      // namespace net
} // namespace ele

#endif
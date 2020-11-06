#include "signal_queue.h"

#include "io_service.h"
#include "self_pipe.h"
#include <lib/ele/base/exception.h>

namespace ele
{
    namespace net
    {
        class SignalQueue::Impl
        {
        public:
            using RecvSignalCallback = SignalQueue::RecvSignalCallback;

            Impl(IOService &io_service);
            ~Impl();

            IOService *getIOService() const;
            void setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb);

            void push(int signum);

        private:
            void pipeReadCallback(IODevice *io_device);

        private:
            IOService *io_service_;
            SelfPipe pipe_;
            RecvSignalCallback recv_signal_cb_;
        };

        SignalQueue::Impl::Impl(IOService &io_service) : io_service_(&io_service)
        {
            if (pipe_.open() == false ||
                pipe_.setNonblock() == false ||
                pipe_.setCloseOnExec() == false)
            {
                throw Exception("create signal queue failed in self pipe init");
            }

            pipe_.setReadCallback(std::bind(&SignalQueue::Impl::pipeReadCallback,
                                            this, std::placeholders::_1));

            pipe_.attachIOService(*io_service_);
        }

        SignalQueue::Impl::~Impl()
        {
        }

        IOService *SignalQueue::Impl::getIOService() const
        {
            return io_service_;
        }

        void SignalQueue::Impl::setRecvSignalCallback(const RecvSignalCallback &recv_signal_cb)
        {
            recv_signal_cb_ = recv_signal_cb;
        }

        void SignalQueue::Impl::push(int signum)
        {
            pipe_.write((const char *)&signum, sizeof(signum));
        }

        void SignalQueue::Impl::pipeReadCallback(IODevice *io_device)
        {
            int signum = 0;

            while (pipe_.read((char *)&signum, sizeof(int)) > 0)
            {
                if (recv_signal_cb_)
                {
                    recv_signal_cb_(signum);
                }
            }
        }

        SignalQueue::SignalQueue(IOService &io_service) : pimpl_(new Impl(io_service))
        {
        }

        SignalQueue::~SignalQueue()
        {
        }

        IOService *SignalQueue::getIOService() const
        {
            return pimpl_->getIOService();
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
} // namespace ele
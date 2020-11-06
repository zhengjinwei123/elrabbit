#ifndef ELE_NET_MESSAGE_QUEUE_H
#define ELE_NET_MESSAGE_QUEUE_H

#include <cstddef>
#include <lib/ele/base/class_util.h>
#include <lib/ele/base/concurrent_queue.h>
#include <lib/ele/base/exception.h>
#include <lib/ele/net/io_service.h>
#include <lib/ele/net/self_pipe.h>

#include <functional>

namespace ele
{
    namespace net
    {
        template <class T>
        class MessageQueue
        {
        public:
            using RecvMessageCallback = std::function<void(MessageQueue *)>;

            explicit MessageQueue(IOService &io_service, size_t max_size = 0) : io_service_(&io_service), queue_(max_size)
            {
                if (pipe_.open() == false ||
                    pipe_.setNonblock() == false ||
                    pipe_.setCloseOnExec() == false)
                {
                    throw Exception("create message queue failed in self pipe init");
                }

                pipe_.setReadCallback(std::bind(&MessageQueue::pipeReadCallback, this, std::placeholders::_1));
                pipe_.attachIOService(*io_service_);
            }

            ~MessageQueue()
            {

            }

            IOService *getIOService() const
            {
                return io_service_;
            }

            void setRecvMessageCallback(const RecvMessageCallback &recv_message_cb)
            {
                recv_message_cb_ = recv_message_cb;
            }

            size_t size()
            {
                return queue_.size();
            }

            void push(const T &item)
            {
                queue_.push(item);
                pipe_.write("1", 1);
            }

            bool pop(T &item)
            {
                return queue_.popIfNotEmpty(item);
            }

            bool peek(T &item)
            {
                return queue_.peek(item);
            }

            void yield()
            {
                pipe_.write("1", 1);
            }

        private:
            void pipeReadCallback(IODevice *io_device)
            {
                char buffer[1024];
                while (pipe_.read(buffer, sizeof(buffer)) > 0)
                    ;
                if (recv_message_cb_)
                {
                    recv_message_cb_(this);
                }
            }

        private:
            ELE_NONCOPYABLE(MessageQueue);

            IOService *io_service_;
            ConcurrentQueue<T> queue_;
            SelfPipe pipe_;
            RecvMessageCallback recv_message_cb_;
        };
    } // namespace net
} // namespace ele

#endif
#include "log_async_sink.h"

#include "concurrent_queue.h"
#include "dynamic_buffer.h"
#include "mutex_lock.h"
#include "object_pool.h"
#include "thread.h"
#include <cstring>

namespace ele
{
    class LogAsyncSink::Impl
    {
    public:
        explicit Impl(LogSink *adapted_sink, size_t queue_size);
        ~Impl();

        void log(const char *buffer, size_t size);
        void logThreadFunc();

    private:
        LogSink *adapted_sink_;

        Thread log_thread_;
        ConcurrentQueue<DynamicBuffer *> queue_;

        MutexLock pool_mutex_;
        ObjectPool<DynamicBuffer> pool_;
    };

    LogAsyncSink::Impl::Impl(LogSink *adapted_sink, size_t queue_size) : adapted_sink_(adapted_sink),
                                                                         log_thread_("log_thread"),
                                                                         queue_(queue_size)
    {
        log_thread_.start(std::bind(&LogAsyncSink::Impl::logThreadFunc, this));
    }

    LogAsyncSink::Impl::~Impl()
    {
        queue_.push(NULL);
        log_thread_.join();

        delete adapted_sink_;
    }

    void LogAsyncSink::Impl::log(const char *buffer, size_t size)
    {
        std::unique_ptr<DynamicBuffer> queue_buffer;
        {
            MutexLockGuard lock(pool_mutex_);
            queue_buffer.reset(pool_.getObject());
        }

        queue_buffer->reserveWritableBytes(size);
        ::memcpy(queue_buffer->writeBegin(), buffer, size);
        queue_buffer->write(size);

        queue_.push(queue_buffer.get());
        queue_buffer.release();
    }

    void LogAsyncSink::Impl::logThreadFunc()
    {
        for (;;)
        {
            DynamicBuffer *queue_buffer_raw = nullptr;
            queue_.pop(queue_buffer_raw);

            if (nullptr == queue_buffer_raw)
            {
                break;
            }

            std::unique_ptr<DynamicBuffer> queue_buffer(queue_buffer_raw);

            adapted_sink_->log(queue_buffer->readBegin(), queue_buffer->readableBytes());
            queue_buffer->read(queue_buffer->readableBytes());
            {
                MutexLockGuard lock(pool_mutex_);
                pool_.returnObject(queue_buffer.get());
                queue_buffer.release();
            }
        }
    }

    LogAsyncSink::LogAsyncSink(LogSink *adapted_sink, size_t queue_size) : pimpl_(new Impl(adapted_sink, queue_size))
    {
    }

    LogAsyncSink::~LogAsyncSink()
    {
    }

    void LogAsyncSink::log(const char *buffer, size_t size)
    {
        pimpl_->log(buffer, size);
    }

} // namespace ele
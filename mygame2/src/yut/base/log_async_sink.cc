#include <yut/base/log_async_sink.h>
#include <yut/base/log_sink.h>

#include <cstring>
#include <yut/base/concurrent_queue.h>
#include <yut/base/dynamic_buffer.h>
#include <yut/base/object_pool.h>
#include <mutex>
#include <thread>
#include <memory>

namespace yut {
namespace base {

class LogAsyncSink::Impl {
public:
    Impl(LogSink *adapted_sink, size_t queue_size);
    ~Impl();

    void log(const char *buffer, size_t size);

    void logThreadFunc();

private:
    LogSink *adapted_sink_;
    std::thread log_thread_;
    ConcurrentQueue<DynamicBuffer *> queue_;
    std::mutex pool_mutex_;
    ObjectPool<DynamicBuffer> pool_;
};

///////////////////////////////////////////////////////////
LogAsyncSink::Impl::Impl(LogSink *adapted_sink, size_t queue_size) :
    adapted_sink_(adapted_sink), queue_(queue_size)
{
    log_thread_ = std::thread(std::bind(&LogAsyncSink::Impl::logThreadFunc, this));
}

LogAsyncSink::Impl::~Impl()
{
    queue_.push(nullptr);
    log_thread_.join();

    delete adapted_sink_;
}

void LogAsyncSink::Impl::log(const char *buffer, size_t size)
{
    std::unique_ptr<DynamicBuffer> queue_buffer;
    {
        std::lock_guard<std::mutex> lock(pool_mutex_);
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

        if (nullptr == queue_buffer_raw) {
            break;
        }

        std::unique_ptr<DynamicBuffer> queue_buffer(queue_buffer_raw);

        adapted_sink_->log(queue_buffer->readBegin(), queue_buffer->readableBytes());
        queue_buffer->read(queue_buffer->readableBytes());

        {
            std::lock_guard<std::mutex> lock(pool_mutex_);
            pool_.returnObject(queue_buffer.get());
            queue_buffer.release();
        }
    }
}

///////////////////////////////////////////////////////////////////
LogAsyncSink::LogAsyncSink(LogSink *adapted_sink, size_t queue_size) :
    pimpl_(new Impl(adapted_sink, queue_size))
{
}

LogAsyncSink::~LogAsyncSink()
{
}

void LogAsyncSink::log(const char *buffer, size_t size)
{
    pimpl_->log(buffer, size);
}


} // namespace base
} // namespace yut
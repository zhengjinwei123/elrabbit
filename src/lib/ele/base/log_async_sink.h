#ifndef ELE_BASE_LOG_ASYNC_SINK_H
#define ELE_BASE_LOG_ASYNC_SINK_H

#include "class_util.h"
#include "log_sink.h"
#include <cstddef>
#include <memory>

namespace ele
{
    class LogAsyncSink : public LogSink
    {
    public:
        LogAsyncSink(LogSink *adapted_sink, size_t queue_size = 100);
        virtual ~LogAsyncSink();

        virtual void log(const char *buffer, size_t size);

    private:
        ELE_NONCOPYABLE(LogAsyncSink);

        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };
} // namespace ele

#endif
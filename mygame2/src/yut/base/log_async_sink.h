#ifndef YUT_BASE_LOG_ASYNC_SINK_H
#define YUT_BASE_LOG_ASYNC_SINK_H

#include <cstddef>
#include <yut/base/class_util.h>
#include <yut/base/log_sink.h>
#include <memory>

namespace yut {
namespace base {

class LogAsyncSink : public LogSink {
public:
    LogAsyncSink(LogSink *adapted_sink, size_t queue_size = 100);
    virtual ~LogAsyncSink();

    virtual void log(const char *buffer, size_t size);

private:
    YUT_NONCOPYABLE(LogAsyncSink)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace base
} // namespace yut

#endif
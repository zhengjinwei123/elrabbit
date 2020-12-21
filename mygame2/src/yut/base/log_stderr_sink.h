#ifndef YUT_BASE_LOG_STDERR_SINK_H
#define YUT_BASE_LOG_STDERR_SINK_H

#include <cstddef>
#include <yut/base/class_util.h>
#include <yut/base/log_sink.h>

namespace yut {
namespace base {

class LogStderrSink : public LogSink {
public:
    LogStderrSink();
    virtual ~LogStderrSink();

    virtual void log(const char *buffer, size_t size);

private:
    YUT_NONCOPYABLE(LogStderrSink)
};

} //namespace base
} // namespace yut

#endif
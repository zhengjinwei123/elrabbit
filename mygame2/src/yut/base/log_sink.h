#ifndef YUT_BASE_LOG_SINK_H
#define YUT_BASE_LOG_SINK_H

#include <cstddef>
#include <yut/base/class_util.h>

namespace yut {

namespace base {

class LogSink {
public:
    LogSink() {}
    virtual ~LogSink() {}

    virtual void log(const char *buffer, size_t size) = 0;

private:
    YUT_NONCOPYABLE(LogSink)

};
} // namespace base
} // namespace yut

#endif
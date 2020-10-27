#ifndef ELE_BASE_LOG_STDERR_SINK_H
#define ELE_BASE_LOG_STDERR_SINK_H

#include "class_util.h"
#include "log_sink.h"
#include <cstddef>

namespace ele
{
    class LogStderrSink : public LogSink
    {
    public:
        LogStderrSink();
        virtual ~LogStderrSink();

        virtual void log(const char *buffer, size_t size);

    private:
        ELE_NONCOPYABLE(LogStderrSink);
    };
} // namespace ele
#endif
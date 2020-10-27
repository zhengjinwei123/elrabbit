#ifndef ELE_BASE_LOG_SINK_H
#define ELE_BASE_LOG_SINK_H

#include "noncopyable.h"
#include <cstddef>

namespace ele
{
    class LogSink : noncopyable
    {
    public:
        LogSink() {}
        virtual ~LogSink() {}
        virtual void log(const char *buffer, size_t size) = 0;
    };
} // namespace ele
#endif
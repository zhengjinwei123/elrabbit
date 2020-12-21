#ifndef YUT_BASE_LOG_CORE_H
#define YUT_BASE_LOG_CORE_H

#include <cstddef>
#include <cstdarg>
#include <yut/base/class_util.h>
#include <memory>
#include <functional>

namespace yut {
namespace base {

class LogSink;

class LogCore {
public:
    using LogFormatter = std::function<size_t (char *buffer,
        size_t buffer_size, int level, const char *filename,
        int line, const char *function, const char *format, va_list args)>;
    
    void setMaxLoggerCount(int count = 1);

    void setMaxLogSize(int size = 4096); // 每个log buffer最大字节数， 超过这个大小，就会truncate 出新的buffer

    bool registerLogger(int logger_id, LogFormatter formatter = nullptr, int level_filter = -1);
    void removeLogger(int logger_id);

    bool addSink(int logger_id, LogSink *sink, LogFormatter formatter = nullptr, int level_filter = -1);

    void log(int logger_id, int level, const char *filename, int line, const char *function, const char *format, ...);
    void log(int logger_id, int level, const char *filename, int line, const char *function, const char *format, va_list args);

    void plainLog(int logger_id, int level, const char *format, ...);
    void plainLog(int logger_id, int level, const char *format, va_list args);

    void setLevelFilter(int logger_id, int level_filter);

private:
    YUT_PRECREATED_SINGLETON(LogCore)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};
} // 
} // namespace yut
#endif
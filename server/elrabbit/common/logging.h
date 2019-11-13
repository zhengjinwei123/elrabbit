#ifndef ELRABBIT_COMMON_LOGGING_H
#define ELRABBIT_COMMON_LOGGING_H

#include "elrabbit/common/log_stream.h"
#include "elrabbit/common/timestamp.h"

namespace elrabbit {
namespace common {

class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
        public:
            template<int N>
            SourceFile(const char (&arr)[N]) :
                data_(arr), size_(N-1)
            {
                const char* slash = strrchr(data_, '/');
                if (slash) {
                    data_ = slash + 1;
                    size_ = static_cast<int>(data_ - arr);
                }
            }

            explicit SourceFile(const char* filename) :
                data_(filename)
            {
                const char* slash = strrchr(filename, '/');
                if (slash) {
                    data_ = slash + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

            const char* data_;
            int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;
};

extern Logger::LogLevel g_loglevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_loglevel;
}

#define LOG_STRACE if (elrabbit::common::Logger::logLevel() <= elrabbit::common::Logger::TRACE) \
    elrabbit::common::Logger(__FILE__, __LINE__, elrabbit::common::Logger::TRACE, __func__).stream()

#define LOG_DEBUG if (elrabbit::common::Logger::logLevel() <= elrabbit::common::Logger::DEBUG) \
    elrabbit::common::Logger(__FILE__, __LINE__, elrabbit::common::Logger::DEBUG, __func__).stream()

#define LOG_INFO if (elrabbit::common::Logger::logLevel() <= lrabbit::common::Logger::INFO) \
    elrabbit::common::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN elrabbit::common::Logger(__FILE__, __LINE__, elrabbit::common::Logger::WARN).stream()
#define LOG_ERROR elrabbit::common::Logger(__FILE__, __LINE__, elrabbit::common::Logger::ERROR).stream()
#define LOG_FATAL elrabbit::common::Logger(__FILE__, __LINE__, elrabbit::common::Logger::FATAL).stream()

#define LOG_SYSERR elrabbit::common::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL elrabbit::common::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int saved_errno);

#define CHECK_NOTNULL(val) \
    ::elrabbit::common::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be not NULL", (val))

template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
    if (ptr == NULL) {
        Logger(file, line, Logger::FATAL).stream() << names;
    }
    return ptr;
}

}// namespace common
} // namespace elrabbit

#endif // ELRABBIT_COMMON_LOGGING_H
#ifndef YUT_BASE_BASE_LOGGER_H
#define YUT_BASE_BASE_LOGGER_H

#include <cstdarg>
#include <yut/base/class_util.h>

namespace yut {

namespace base {

class BaseLogger {
public:
    struct LogLevel {
        enum type {
            MIN = 0,

            DEBUG = 0,
            WARNING,
            ERROR,

            MAX
        };
    };

    typedef void (*LogFunc)(
        int level,
        const char *format,
        va_list args
    );

    void setLogFunc(LogFunc log_func);
    void log(int level, const char *format, ...);

private:
    YUT_PRECREATED_SINGLETON(BaseLogger)

    LogFunc log_func_;
};

} // namespace base
} // namespace yut

#define BASE_DEBUG(_format, ...) \
    yut::base::BaseLogger::getInstance()->log(yut::base::BaseLogger::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#define BASE_WARNING(_format, ...) \
    yut::base::BaseLogger::getInstance()->log(yut::base::BaseLogger::LogLevel::WARNING, _format, ##__VA_ARGS__)

#define BASE_ERROR(_format, ...) \
    yut::base::BaseLogger::getInstance()->log(yut::base::BaseLogger::LogLevel::ERROR, _format, ##__VA_ARGS__)


#endif
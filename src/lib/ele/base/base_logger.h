#ifndef ELE_BASE_BASE_LOGGER_H
#define ELE_BASE_BASE_LOGGER_H

#include "class_util.h"
#include <cstdarg>
#include <functional>

namespace ele
{
    class BaseLogger
    {
    public:
        struct LogLevel
        {
            enum type
            {
                MIN = 0,
                DEBUG = 0,
                WARNING,
                ERROR,

                MAX
            };
        };

        using LogFunc = std::function<void(int level, const char *format, va_list args)>;

        void setLogFunc(LogFunc log_func);
        void log(int level, const char *format, ...);

    private:
        ELE_PRECREATED_SINGLETON(BaseLogger);

        LogFunc log_func_;
    }; // BaseLogger
} // namespace ele

#define BASE_DEBUG(_format, ...)         \
    ele::BaseLogger::getInstance()->log( \
        ele::BaseLogger::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#define BASE_WARNING(_format, ...)       \
    ele::BaseLogger::getInstance()->log( \
        ele::BaseLogger::LogLevel::WARNING, _format, ##__VA_ARGS__)

#define BASE_ERROR(_format, ...)         \
    ele::BaseLogger::getInstance()->log( \
        ele::BaseLogger::LogLevel::ERROR, _format, ##__VA_ARGS__)

#endif
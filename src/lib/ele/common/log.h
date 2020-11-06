#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <lib/ele/base/class_util.h>
#include <lib/ele/base/log_core.h>
#include <string>

namespace ele
{
    namespace common
    {
        namespace log
        {
            struct LogLevel
            {

                enum type
                {
                    MIN = 0,

                    DEBUG = 0,
                    INFO,
                    WARNING,
                    ERROR,

                    MAX,
                };
            };

            struct Logger
            {
                enum type
                {
                    MIN = 0,

                    MAIN = 0,
                    RESOURCE_ITEM = 1,
                    ACTION = 2,

                    MAX,
                };
            };

            class LogManager
            {
            public:
                bool initMainLogger(const std::string &log_file_path, bool log_stderr);
                bool initResourceItemLogger(const std::string &log_file_path);
                bool initActionLogger(const std::string &log_file_path);

            private:
                ELE_SINGLETON(LogManager)

            }; // class LoggerManager

            void logNetMessage(const char *format, ...);

        } // namespace log
    }     // namespace common
} // namespace ele

#define LOG_DEBUG(_format, ...)                                                                  \
    ele::LogCore::getInstance()->log(ele::common::log::Logger::MAIN,                                  \
                                     ele::common::log::LogLevel::DEBUG, __FILE__, __LINE__, __func__, \
                                     _format, ##__VA_ARGS__)

#define LOG_INFO(_format, ...)                                                                  \
    ele::LogCore::getInstance()->log(ele::common::log::Logger::MAIN,                                 \
                                     ele::common::log::LogLevel::INFO, __FILE__, __LINE__, __func__, \
                                     _format, ##__VA_ARGS__)

#define LOG_WARNING(_format, ...)                                                                  \
    ele::LogCore::getInstance()->log(ele::common::log::Logger::MAIN,                                    \
                                     ele::common::log::LogLevel::WARNING, __FILE__, __LINE__, __func__, \
                                     _format, ##__VA_ARGS__)

#define LOG_ERROR(_format, ...)                                                                  \
    ele::LogCore::getInstance()->log(ele::common::log::Logger::MAIN,                                  \
                                     ele::common::log::LogLevel::ERROR, __FILE__, __LINE__, __func__, \
                                     _format, ##__VA_ARGS__)

#define PLAIN_LOG_DEBUG(_format, ...)                                \
    ele::LogCore::getInstance()->plainLog(ele::common::log::Logger::MAIN, \
                                          ele::common::log::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#define PLAIN_LOG_INFO(_format, ...)                                 \
    ele::LogCore::getInstance()->plainLog(ele::common::log::Logger::MAIN, \
                                          ele::common::log::LogLevel::INFO, _format, ##__VA_ARGS__)

#define PLAIN_LOG_WARNING(_format, ...)                              \
    ele::LogCore::getInstance()->plainLog(ele::common::log::Logger::MAIN, \
                                          ele::common::log::LogLevel::WARNING, _format, ##__VA_ARGS__)

#define PLAIN_LOG_ERROR(_format, ...)                                \
    ele::LogCore::getInstance()->plainLog(ele::common::log::Logger::MAIN, \
                                          ele::common::log::LogLevel::ERROR, _format, ##__VA_ARGS__)

#define LOG_MESSAGE(_format, ...) \
    ele::common::log::logNetMessage(_format, ##__VA_ARGS__)

#define LOG_RESOURCE_ITEM(_format, ...)     \
    ele::LogCore::getInstance()->plainLog(  \
        ele::common::log::Logger::RESOURCE_ITEM, \
        ele::common::log::LogLevel::DEBUG,       \
        _format, ##__VA_ARGS__)

#define LOG_ACTION(_format, ...)           \
    ele::LogCore::getInstance()->plainLog( \
        ele::common::log::Logger::ACTION,       \
        ele::common::log::LogLevel::DEBUG,      \
        _format, ##__VA_ARGS__)

#endif // COMMON_LOG_H

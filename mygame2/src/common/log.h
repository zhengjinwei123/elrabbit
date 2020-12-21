#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <string>
#include <yut/base/class_util.h>
#include <yut/base/log_core.h>

namespace common {
namespace log {

struct LogLevel {
    enum type {
        MIN = 0,

        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,

        MAX,
    };
};

struct Logger {
    enum type {
        MIN = 0,

        MAIN = 0,
        RESOURCE_ITEM = 1,
        ACTION = 2,

        MAX,
    };
};

class LogManager {
public:
    bool initMainLogger(const std::string &log_file_path, bool log_stderr);
    bool initResourceItemLogger(const std::string &log_file_path);
    bool initActionLogger(const std::string &log_file_path);

private:
    YUT_SINGLETON(LogManager)
};

void logNetMessage(const char *format, ...);

} // namespace log
} // namespace common

#define LOG_DEBUG(_format, ...) \
    yut::base::LogCore::getInstance()->log(common::log::Logger::MAIN, \
        common::log::LogLevel::DEBUG, __FILE__, __LINE__, __func__, \
        _format, ##__VA_ARGS__)

#define LOG_INFO(_format, ...) \
    yut::base::LogCore::getInstance()->log(common::log::Logger::MAIN, \
        common::log::LogLevel::INFO, __FILE__, __LINE__, __func__, \
        _format, ##__VA_ARGS__)

#define LOG_WARNING(_format, ...) \
    yut::base::LogCore::getInstance()->log(common::log::Logger::MAIN, \
        common::log::LogLevel::WARNING, __FILE__, __LINE__, __func__, \
        _format, ##__VA_ARGS__)

#define LOG_ERROR(_format, ...) \
    yut::base::LogCore::getInstance()->log(common::log::Logger::MAIN, \
        common::log::LogLevel::ERROR, __FILE__, __LINE__, __func__, \
        _format, ##__VA_ARGS__)

#define PLAIN_LOG_DEBUG(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog(common::log::Logger::MAIN, \
        common::log::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#define PLAIN_LOG_INFO(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog(common::log::Logger::MAIN, \
        common::log::LogLevel::INFO, _format, ##__VA_ARGS__)

#define PLAIN_LOG_WARNING(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog(common::log::Logger::MAIN, \
        common::log::LogLevel::WARNING, _format, ##__VA_ARGS__)

#define PLAIN_LOG_ERROR(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog(common::log::Logger::MAIN, \
        common::log::LogLevel::ERROR, _format, ##__VA_ARGS__)

#define LOG_MESSAGE(_format, ...) \
    common::log::logNetMessage(_format, ##__VA_ARGS__)

#define LOG_RESOURCE_ITEM(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog( \
        common::log::Logger::RESOURCE_ITEM, \
        common::log::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#define LOG_ACTION(_format, ...) \
    yut::base::LogCore::getInstance()->plainLog( \
        common::log::Logger::ACTION, \
        common::log::LogLevel::DEBUG, _format, ##__VA_ARGS__)

#endif

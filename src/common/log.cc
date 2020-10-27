#include "log.h"

#include <cstdarg>
#include <cstdio>
#include <memory>

#include <lib/ele/base/base_logger.h>
#include <lib/ele/base/log_async_sink.h>
#include <lib/ele/base/log_file_sink.h>
#include <lib/ele/base/log_stderr_sink.h>
#include <lib/ele/base/timestamp.h>

namespace common
{
    namespace log
    {
        using ele::BaseLogger;
        using ele::LogAsyncSink;
        using ele::LogCore;
        using ele::LogFileSink;
        using ele::LogStderrSink;
        using ele::Timestamp;

        static size_t logFormatter(char *buffer, size_t buffer_size, int level,
                                   const char *filename, int line, const char *function,
                                   const char *format, va_list args)
        {
            if (level < LogLevel::MIN || level >= LogLevel::MAX)
            {
                return 0;
            }

            static const char *log_level_string[] = {
                "D", "I", "*WARN*", "**ERROR**"};

            Timestamp now;
            now.setNow();

            char time_buffer[1024];
            Timestamp::format(time_buffer, sizeof(time_buffer),
                              "%H:%M:%S", now.getSecond());

            size_t count = 0;

            do
            {
                count += ::snprintf(buffer, buffer_size, "[%s][%s:%03ld][%s:%d](%s)",
                                    log_level_string[level], time_buffer, now.getMilliSecond(),
                                    filename, line, function);

                if (count >= buffer_size)
                {
                    break;
                }

                count += ::vsnprintf(buffer + count, buffer_size - count, format, args);
                if (count >= buffer_size)
                {
                    break;
                }

                count += ::snprintf(buffer + count, buffer_size - count, "\n");
            } while (0);

            if (count >= buffer_size)
            {
                if (buffer_size >= 2)
                {
                    buffer[buffer_size - 2] = '\n';
                }
                return buffer_size - 1;
            }

            return count;
        }

        static void baseLogFunc(int level, const char *format, va_list args)
        {
            static const int log_level_map[] = {
                ele::BaseLogger::LogLevel::DEBUG,
                ele::BaseLogger::LogLevel::WARNING,
                ele::BaseLogger::LogLevel::ERROR};

            ele::LogCore::getInstance()->log(Logger::MAIN, log_level_map[level], "", 0, "base_log", format, args);
        }

        LogManager::LogManager()
        {
            LogCore::getInstance()->setMaxLoggerCount(Logger::MAX);
        }

        LogManager::~LogManager()
        {
        }

        bool LogManager::initMainLogger(const std::string &log_file_path, bool log_stderr)
        {
            if (LogCore::getInstance()->registerLogger(Logger::MAIN, logFormatter) == false)
            {
                return false;
            }

            if (log_file_path.empty() == false)
            {
                std::unique_ptr<LogFileSink> file_sink(new LogFileSink(log_file_path));
                if (file_sink->openFile() == false) {
                    return false;
                }

                std::unique_ptr<LogAsyncSink> async_sink(new LogAsyncSink(file_sink.get(), 1024));
                file_sink.release();

                if (LogCore::getInstance()->addSink(Logger::MAIN, async_sink.get()) == false) {
                    return false;
                }

                async_sink.release();
            }

            if (log_stderr)
            {
                std::unique_ptr<LogStderrSink> stderr_sink(new LogStderrSink());
                if (LogCore::getInstance()->addSink(Logger::MAIN, stderr_sink.get()) == false)
                {
                    return false;
                }
                stderr_sink.release();
            }

            BaseLogger::getInstance()->setLogFunc(baseLogFunc);

            return true;
        }

        bool LogManager::initResourceItemLogger(const std::string &log_file_path)
        {
            if (LogCore::getInstance()->registerLogger(Logger::RESOURCE_ITEM) == false)
            {
                return false;
            }
            if (log_file_path.empty() == false)
            {
                std::unique_ptr<LogFileSink> file_sink(new LogFileSink(log_file_path));
                if (file_sink->openFile() == false)
                {
                    return false;
                }
                std::unique_ptr<LogAsyncSink> async_sink(new LogAsyncSink(file_sink.get(), 1024));
                file_sink.release();

                if (LogCore::getInstance()->addSink(Logger::RESOURCE_ITEM, async_sink.get()) == false)
                {
                    return false;
                }
                async_sink.release();
            }

            return true;
        }

        bool LogManager::initActionLogger(const std::string &log_file_path)
        {
            if (LogCore::getInstance()->registerLogger(Logger::ACTION) == false)
            {
                return false;
            }

            if (log_file_path.empty() == false)
            {
                std::unique_ptr<LogFileSink> file_sink(new LogFileSink(log_file_path));
                if (file_sink->openFile() == false)
                {
                    return false;
                }

                std::unique_ptr<LogAsyncSink> async_sink(new LogAsyncSink(file_sink.get(), 1024));
                file_sink.release();

                if (LogCore::getInstance()->addSink(Logger::ACTION, async_sink.get()) == false)
                {
                    return false;
                }

                async_sink.release();
            }

             return true;
        }

        void logNetMessage(const char *format, ...)
        {
            Timestamp now;
            now.setNow();

            char time_buffer[1024];
            Timestamp::format(time_buffer, sizeof(time_buffer), "%H:%M:%S", now.getSecond());

            char wrap_format[1024];
            snprintf(wrap_format, sizeof(wrap_format), "[M][%s:%03ld] %s\n", time_buffer, now.getMilliSecond(), format);

            va_list args;
            va_start(args, format);
            ele::LogCore::getInstance()->plainLog(common::log::Logger::MAIN,
                common::log::LogLevel::INFO, wrap_format, args);
            va_end(args);
        }
    } // namespace log
} // namespace common

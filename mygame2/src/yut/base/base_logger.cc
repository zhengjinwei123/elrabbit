#include <yut/base/base_logger.h>

#include <cstdio>

namespace yut {

namespace base {


YUT_PRECREATED_SINGLETON_IMPL(BaseLogger)

static void defaultLogFunc(int level, const char *format, va_list args)
{
    if (level < BaseLogger::LogLevel::MIN ||
        level >= BaseLogger::LogLevel::MAX) {
        return;
    }

    static const char *log_level_str[] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
    };

    ::fprintf(stderr, "[%s]", log_level_str[level]);
    ::vfprintf(stderr, format, args);
    ::fprintf(stderr, "\n");
}

BaseLogger::BaseLogger() :
    log_func_(defaultLogFunc)
{

}

BaseLogger::~BaseLogger()
{

}

void BaseLogger::setLogFunc(LogFunc log_func)
{
    log_func_ = log_func;
}

void BaseLogger::log(int level, const char *format, ...)
{
#ifdef YUT_BUILD_ENABLE_BASE_LOG
    va_list args;
    va_start(args, format);
    log_func_(level, format, args);
    va_end(args);
#endif
}


} // namespace base
} // namespace yut
#include "elrabbit/common/logging.h"

#include "elrabbit/common/current_thread.h"
#include "elrabbit/common/timestamp.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sstream>

namespace elrabbit {

namespace common {

__thread char t_errno_buf[512];
__thread char t_time[64];
__thread time_t t_last_second;

const char* strerror_tl(int saved_errno)
{
    return strerror_r(saved_errno, t_errno_buf, sizeof(t_errno_buf));
}

Logger::LogLevel initLogLevel()
{
    if (::getenv("ELRABBIT_LOG_TRACE")) {
        return Logger::TRACE;
    } else if (::getenv("ELRABBIT_LOG_DEBUG")) {
        return Logger::DEBUG;
    }
    return Logger::INFO;
}

Logger::LogLevel g_loglevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = 
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "FATAL"
};

class T {
    public:
        T(const char* str, unsigned len) :
            str_(str),
            len_(len) 
        {
            assert(strlen(str) == len);            
        }

        const char* str_;
        const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v.data_, v.size_);
    return s;
}

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);

    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

} // namespace common
} // namespace elrabbit

using namespace elrabbit;
using namespace elrabbit::common;

Logger::Impl::Impl(LogLevel level, int saved_errno, const SourceFile& file, int line) :
    time_(Timestamp::nowTimestamp()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file)
{
    formatTime();
    current_thread::tid();
    stream_ << T(current_thread::tidString(), current_thread::tidStringLength());
    stream_ << T(LogLevelName[level], 6);
    if (saved_errno != 0) {
        stream_ << strerror_tl(saved_errno) << " (errno=" << saved_errno << ") ";
    }
}

void Logger::Impl::formatTime()
{
    Timestamp ts;
    ts.setNow();
    time_t now = ts.getSecond();
    char date_string[256];
    Timestamp::format(date_string, sizeof(date_string),
        "%a, %d %b %Y %H:%M:%S %Z", now);
    stream_ << T(date_string, sizeof(date_string));
}

void Logger::Impl::finish()
{
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line) :
    impl_(INFO, 0, file, line)
{

}

Logger::Logger(SourceFile file, int line, LogLevel level, const char *func) :
    impl_(level, 0, file, line)
{
    impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level) :
    impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort) :
    impl_(toAbort ? FATAL : ERROR, errno, file, line)
{

}

Logger::~Logger()
{
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (impl_.level_ == FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_loglevel = level;
}

void Logger::setOutput(OutputFunc out)
{
    g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
    g_flush = flush;
}
#include <yut/base/log_stderr_sink.h>

#include <cstdio>

namespace yut {
namespace base {

LogStderrSink::LogStderrSink()
{
}

LogStderrSink::~LogStderrSink()
{
}

void LogStderrSink::log(const char *buffer, size_t size)
{
    ::fwrite(buffer, size, 1, stderr);
}

}    
}
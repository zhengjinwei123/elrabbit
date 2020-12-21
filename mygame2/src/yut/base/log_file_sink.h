#ifndef YUT_BASE_LOG_FILE_SINK_H
#define YUT_BASE_LOG_FILE_SINK_H

#include <cstddef>
#include <string>

#include <yut/base/log_sink.h>
#include <yut/base/class_util.h>

#include <memory>

namespace yut {

namespace base {

class LogFileSink : public LogSink {
public:
    LogFileSink(const std::string &file_path);
    virtual ~LogFileSink();

    virtual void log(const char *buffer, size_t size);
    
    bool openFile();

private:
    YUT_NONCOPYABLE(LogFileSink)

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace base
} // namespace yut
#endif
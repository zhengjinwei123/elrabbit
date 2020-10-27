#ifndef ELE_BASE_LOG_FILE_SINK_H
#define ELE_BASE_LOG_FILE_SINK_H

#include <cstddef>
#include <string>
#include <memory>

#include "log_sink.h"
#include "class_util.h"

namespace ele
{
    class LogFileSink : public LogSink {
        public:
            LogFileSink(const std::string &file);
            virtual ~LogFileSink();

            virtual void log(const char *buffer, size_t size);
            bool openFile();

        private:
            ELE_NONCOPYABLE(LogFileSink);
        
            class Impl;
            std::unique_ptr<Impl> pimpl_;
    };
} // namespace ele

#endif
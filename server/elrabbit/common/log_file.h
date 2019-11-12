#ifndef ELRABBIT_COMMON_LOG_FILE_H
#define ELRABBIT_COMMON_LOG_FILE_H

#include <string>
#include <memory>
#include "elrabbit/common/mutex_lock.h"
#include "elrabbit/common/class_util.h"

namespace elrabbit {

namespace file_util {
    class AppendFile;
} // namespace file_util

namespace common {

class LogFile: Noncopyable {
public:
    LogFile(const std::string& base_name,
            off_t roll_size,
            bool thread_safe = true,
            int flush_interval = 3,
            int check_evey_n = 1024);

    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void appendUnlocked(const char* logline, int len);
    static std::string getLogFileName(const std::string& base_name, time_t* now);

private:
    const std::string base_name_;
    const off_t roll_size_;
    const int flush_interval_;
    const int check_every_n_;
    int count_;
    std::unique_ptr<MutexLock> mutex_lock_;
    time_t start_of_period_;
    time_t last_roll_;
    time_t last_flush_;

    std::unique_ptr<file_util::AppendFile> file_;

    const static int k_roll_per_seconds_ = 60 * 60 * 24;
};

} // namespace common
} // namespace elrabbit
#endif // ELRABBIT_COMMON_LOG_FILE_H
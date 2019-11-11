#ifndef ELRABBIT_COMMON_PROCESS_INFO_H
#define ELRABBIT_COMMON_PROCESS_INFO_H

#include <string>
#include <vector>
#include "elrabbit/common/timestamp.h"
#include "elrabbit/common/string_piece.h"


namespace elrabbit {

using common::StringPiece;
using common::Timestamp;

namespace process_info {

    pid_t pid();
    std::string pidString();
    uid_t uid();
    std::string userName();
    uid_t euid();
    Timestamp startTime();
    int clockTickPerSecond();
    int pageSize();
    bool idDebugBuild();

    std::string hostName();
    std::string procName();
    StringPiece procName(const std::string &stat);

    // read /proc/self/status
    std::string procStatus();
    // read /proc/self/stat
    std::string procStat();
    // read /proc/self/task/tid/stat
    std::string threadStat();

    std::string exePath();
    int openedFiles();
    int maxOpenFiles();

    struct CpuTime
    {
        double user_seconds;
        double system_seconds;

        CpuTime() : user_seconds(0.0), system_seconds(0.0) {}
        double total() const { return user_seconds + system_seconds; }
    };

    CpuTime cpuTime();
    int numThreads();
    std::vector<pid_t> threads();

} // namespace common
} // namespace elrabbit
#endif // ELRABBIT_COMMON_PROCESS_INFO_H
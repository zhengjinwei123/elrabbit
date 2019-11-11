#include "elrabbit/common/process_info.h"
#include "elrabbit/common/current_thread.h"
#include "elrabbit/common/file_util.h"

#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>

namespace elrabbit {

using common::Timestamp;
using common::StringPiece;

__thread int t_open_files_num = 0;
int fdDirFilter(const struct dirent* d)
{
    if (::isdigit(d->d_name[0])) {
        ++ t_open_files_num;
    }
    return 0;
}

__thread std::vector<pid_t>* t_pids = nullptr;
int taskDirFilter(const struct dirent* d)
{
    if (::isdigit(d->d_name[0])) {
        t_pids->push_back(atoi(d->d_name));
    }
    return 0;
}

int scanDir(const char* dir_path, int (*filter)(const struct dirent*))
{
    struct dirent** name_list = nullptr;
    int result = ::scandir(dir_path, &name_list, filter, alphasort);
    assert(name_list == nullptr);
    return result;
}

Timestamp g_start_time = Timestamp::nowTimestamp();
int g_clock_ticks = static_cast<int>(::sysconf(_SC_CLK_TCK));
int g_page_size = static_cast<int>(::sysconf(_SC_PAGE_SIZE));


namespace process_info {

pid_t pid()
{
    return ::getpid();
}

std::string pidString()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", pid());
    return buf;
}

uid_t uid()
{
    return ::getuid();
}

std::string userName()
{
    struct passwd pwd;
    struct passwd* result = nullptr;
    char buf[8192];
    const char* name = "unknow_user";

    getpwuid_r(uid(), &pwd, buf, sizeof(buf), &result);
    if (result) {
        name = pwd.pw_name;
    }
    return name;
}

uid_t euid()
{
    return ::geteuid();
}

Timestamp startTime()
{
    return g_start_time;
}

int clockTickPerSecond()
{
    return g_clock_ticks;
}

int pageSize()
{
    return g_page_size;
}

std::string hostName()
{
    char buf[256];
    if (::gethostname(buf, sizeof buf) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    return "unknown_host";
}

std::string procName()
{
    return procName(procStat()).as_string();
}

StringPiece procName(const std::string& stat)
{
    StringPiece name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');
    if (lp != std::string::npos && rp != std::string::npos && lp < rp) {
        name.set(stat.data() + lp + 1, static_cast<int>(rp - lp -1));
    }
    return name;
}

std::string procStatus()
{
    std::string result;
    file_util::readFile("/proc/self/status", 65536, &result);
    return result;
}

std::string procStat()
{
    std::string result;
    file_util::readFile("/proc/self/stat", 65536, &result);
    return result;
}

std::string threadStat()
{
    char buf[64];
    snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", current_thread::tid());
    std::string result;
    file_util::readFile(buf, 65536, &result);
    return result;
}

std::string exePath()
{
    std::string result;
    char buf[1024];
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
    if (n > 0) {
        result.assign(buf, n);
    }
    return result;
}

int openedFiles()
{
    t_open_files_num = 0;
    scanDir("/proc/self/fd", fdDirFilter);
    return t_open_files_num;
}

int maxOpenFiles()
{
    struct rlimit rl;
    if (::getrlimit(RLIMIT_NOFILE, &rl)) {
        return openedFiles();
    }
    return static_cast<int>(rl.rlim_cur);
}

CpuTime cpuTime()
{
    CpuTime t;
    struct tms tms;
    if (::times(&tms) >= 0) {
        const double hz = static_cast<double>(clockTickPerSecond());
        t.user_seconds = static_cast<double>(tms.tms_utime) / hz;
        t.system_seconds = static_cast<double>(tms.tms_stime) / hz;
    }
    return t;
}

int numThreads()
{
    int result = 0;
    std::string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos) {
        result = ::atoi(status.c_str() + pos + 8);
    }
    return result;
}

std::vector<pid_t> threads()
{
    std::vector<pid_t> result;
    t_pids = &result;
    scanDir("/proc/self/task", taskDirFilter);
    t_pids = nullptr;
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace process_info
} // namespace elrabbit





#include "elrabbit/common/process_info.h"
#include "elrabbit/common/current_thread.h"
#include "elrabbit/common/file_util.h"

#include <algorithm>
#include <assert.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>

namespace elrabbit {

namespace process_info {

__thread int t_open_files_num = 0;
int fdDirFilter(const struct dirent* d)
{
    if (::isdigit(d->d_name[0])) {
        ++ t_open_files_num;
    }
    return 0;
}

__thread std::vector<pid_t>* t_pids = nullptr;




} // namespace process_info
} // namespace elrabbit





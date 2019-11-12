#include "elrabbit/common/log_file.h"

#include "elrabbit/common/file_util.h"
#include "elrabbit/common/process_info.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace elrabbit::common;

LogFile::LogFile(const std::string& base_name,
                 off_t roll_size,
                 bool thread_safe,
                 int flush_interval,
                 int check_every_n)
    : base_name_(base_name),
      roll_size_(roll_size),
      flush_interval_(flush_interval),
      check_every_n_(check_every_n),
      count_(0),
      mutex_lock_(thread_safe ? new MutexLock() : nullptr),
      start_of_period_(0),
      last_roll_(0),
      last_flush_(0)
{
    assert(base_name.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char* logline, int len)
{
    if (mutex_lock_)
    {
        MutexLockGuard lock(*mutex_lock_);
        appendUnlocked(logline, len);
    } else {
        appendUnlocked(logline, len);
    }
}


void LogFile::flush()
{
    if (mutex_lock_) {
        MutexLockGuard lock(*mutex_lock_);
        file_->flush();
    } else {
        file_->flush();
    }
}

void LogFile::appendUnlocked(const char* logline, int len)
{
    file_->append(logline, len);
    if (file_->writtenBytes() > roll_size_) {
        rollFile();
    } else {
        ++count_;
        if (count_ >= check_every_n_) {
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t this_period = now / k_roll_per_seconds_ * k_roll_per_seconds_;
            if (this_period != start_of_period_) {
                rollFile();
            } else if(now - last_flush_ > flush_interval_){
                last_flush_ = now;
                file_->flush();
            }
        }
    }
}

bool LogFile::rollFile()
{
    time_t now = 0;
    std::string file_name = getLogFileName(base_name_, &now);
    time_t start = now / k_roll_per_seconds_ * k_roll_per_seconds_;

    if (now > last_roll_) {
        last_roll_ = now;
        last_flush_ = now;
        start_of_period_ = start;
        file_.reset(new file_util::AppendFile(file_name));
        return true;
    }
    return false;
}

std::string LogFile::getLogFileName(const std::string& base_name, time_t *now)
{
    std::string file_name;
    file_name.reserve(base_name.size() + 64);
    file_name = base_name;

    char timebuf[32];
    struct tm tm;
    *now = time(nullptr);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
    file_name += timebuf;

    file_name += process_info::hostName();

    char pidbuf[32];
    snprintf(pidbuf, sizeof(pidbuf), ".%d", process_info::pid());
    file_name += pidbuf;
    file_name += ".log";

    return file_name;
}
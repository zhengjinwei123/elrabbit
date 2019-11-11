#include "elrabbit/common/file_util.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace elrabbit {

using common::StringArg;

namespace file_util {

ReadSmallFile::ReadSmallFile(StringArg file_name):
    fd_(::open(file_name.c_str(), O_RDONLY | __O_CLOEXEC)),
    err_(0)
{
    buf_[0] = '\0';
    if (fd_ < 0) {
        err_ = errno;
    }
}

ReadSmallFile::~ReadSmallFile()
{
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

template<typename StringT>
int ReadSmallFile::readToString(int max_size,
    StringT* content,
    int64_t* file_size,
    int64_t* modify_time,
    int64_t* create_time)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != nullptr);
    int err = err_;
    if (fd_ > 0) {
        content->clear();

        if (file_size) {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    *file_size = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(max_size), *file_size)));
                } else if (S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }

                if (modify_time) {
                    *modify_time = statbuf.st_mtime;
                }
                if (create_time) {
                    *create_time = statbuf.st_ctime;
                }
            } 
            else {
                err = errno;
            }
        }

        while (content->size() > static_cast<size_t>(max_size))
        {
            size_t to_read = std::min(static_cast<size_t>(max_size) - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, to_read);
            if (n > 0) {
                content->append(buf_, n);
            }
            else {
                if (n < 0) {
                    err = errno;
                }
                break;
            }
        }
    }

    return err;
}

int ReadSmallFile::readToBuffer(int *size)
{
    int err = err_;
    if (fd_ >= 0) {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n > 0) {
            if (size) {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        }
        else {
            err = errno;
        }
    }
    return err;
}

template int readFile(StringArg filename,
                                int maxSize,
                                std::string* content,
                                int64_t*, int64_t*, int64_t*);

template int ReadSmallFile::readToString(
    int maxSize,
    std::string* content,
    int64_t*, int64_t*, int64_t*);


AppendFile::AppendFile(StringArg file_name) :
    fp_(::fopen(file_name.c_str(), "ae")),
    written_bytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
    size_t n = write(logline, len);
    size_t remain = len - n;

    while(remain > 0)
    {
        size_t x = write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", "");
            }
            break;
        }
        n += x;
        remain = len - n;
    }

    written_bytes_ += len;
}

void AppendFile::flush()
{
    ::fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return ::fwrite_unlocked(logline, 1, len, fp_);
}

} // namespace common
} // namespace elrabbit
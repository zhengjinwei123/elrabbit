#ifndef ELRABBIT_COMMON_FILE_UTIL_H
#define ELRABBIT_COMMON_FILE_UTIL_H

#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include "elrabbit/common/class_util.h"
#include "elrabbit/common/string_piece.h"

namespace elrabbit {

namespace common {

// read small file size < 64KB
class ReadSmallFile : Noncopyable {
public:
    ReadSmallFile(StringArg file_name);
    ~ReadSmallFile();

    template<typename StringT>
    int readToString(int max_size,
        StringT* content,
        int64_t* file_size,
        int64_t* modify_time,
        int64_t* create_time
    );

    int readToBuffer(int* size);

    const char* buffer() const { return buf_; }

    static const int BUFFER_SIZE = 64*1024;
private:
    int fd_;
    int err_;
    char buf_[BUFFER_SIZE];
};

template<typename StringT>
int readFile(StringArg file_name,
             int max_size,
             StringT* content,
             int64_t* file_size = nullptr,
             int64_t* modify_time = nullptr,
             int64_t* create_time = nullptr)
{
    ReadSmallFile file(file_name);
    return file.readToString(max_size, content, file_size, modify_time, create_time);
}

class AppendFile : Noncopyable {
public:
    explicit AppendFile(StringArg file_name);
    ~AppendFile();

    void append(const char* logline, size_t len);
    void flush();
    off_t writtenBytes() const { return written_bytes_;}

    static const int BUFFER_SIZE = 64*1024;
private:
    size_t write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[BUFFER_SIZE];
    off_t written_bytes_;
};

} // namespace common
} // namespace elrabbit
#endif // ELRABBIT_COMMON_FILE_UTIL_H
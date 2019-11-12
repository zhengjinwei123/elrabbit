#ifndef ELRABBIT_COMMON_TIMESTAMP_H
#define ELRABBIT_COMMON_TIMESTAMP_H

#include <stdint.h>
#include <cstddef>
#include <ctime>

namespace elrabbit {

namespace common {

class Timestamp {
public:
    Timestamp();
    Timestamp(time_t second, int64_t nanosecond);
    ~Timestamp();

    void setNow();

    time_t getSecond() const { return second_; }
    int64_t getMillisecond() const { return nanosecond_ / 1000000; }

    Timestamp &operator+=(int64_t millisecond);
    bool operator<(const Timestamp &other) const;
    Timestamp operator+(int64_t millisecond) const;

    bool millisecondLess(const Timestamp &other) const;
    bool millisecondEqual(const Timestamp &other) const;

    int64_t distanceSecond(const Timestamp &other) const;
    int64_t distanceMillisecond(const Timestamp &other) const;

    static time_t now();
    static Timestamp nowTimestamp();
    static size_t format(char* buffer, size_t size, const char* format, time_t second);

public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    time_t second_;
    int64_t nanosecond_;
};

} // namespace common
} // namespace elrabbit


#endif // ELRABBIT_COMMON_TIMESTAMP_H
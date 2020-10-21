#ifndef ELE_BASE_TIMESTAMP_H
#define ELE_BASE_TIMESTAMP_H

#include <cstddef>
#include <ctime>
#include <stdint.h>

namespace ele
{
    class Timestamp
    {
    public:
        Timestamp();
        ~Timestamp();

        void setNow();

        time_t getSecond() const { return second_; }
        int64_t getNanoSecond() const { return nanosecond_; }
        int64_t getMilliSecond() const { return nanosecond_ / 1000000; }

        Timestamp &operator +=(int64_t millisecond);
        bool operator<(const Timestamp &other) const;
        Timestamp operator+(int64_t millosecond) const;

        bool millisecondLess(const Timestamp &other) const;
        bool millisecondEqual(const Timestamp &other) const;

        int64_t distanceSecond(const Timestamp &other) const;
        int64_t distanceMillisecond(const Timestamp &other) const;

        static time_t now();
        static size_t format(char *buffer, size_t size, const char *format, time_t second);

    private:
        time_t second_;
        int64_t nanosecond_;

    }; // Timestamp
} // namespace ele
#endif // ELE_BASE_TIMESTAMP_H
#include <yut/base/timestamp.h>
#include <sys/times.h>


namespace yut {

namespace base {

Timestamp::Timestamp() :
    second_(0),
    nanosecond_(0)
{

}

Timestamp::~Timestamp()
{

}

void Timestamp::setNow()
{
    struct timespec tv;
    ::clock_gettime(CLOCK_REALTIME, &tv);

    second_ = tv.tv_sec;
    nanosecond_ = tv.tv_nsec;
}

Timestamp &Timestamp::operator+=(int64_t millisecond)
{
    second_ += millisecond / 1000;
    nanosecond_ += millisecond % 1000 * 1000000;
    if (nanosecond_ >= 1000000000) {
        second_ += 1;
        nanosecond_ -= 1000000000;
    }

    return *this;
}

bool Timestamp::operator<(const Timestamp &other) const
{
    if (second_ != other.second_) {
        return second_ < other.second_;
    }
    return nanosecond_ < other.nanosecond_;
}

Timestamp Timestamp::operator+(int64_t millisecond) const
{
    Timestamp t(*this);
    t += millisecond;

    return t;
}

bool Timestamp::millisecondLess(const Timestamp &other) const
{
    if (second_ != other.second_) {
        return second_ < other.second_;
    }
    return getMillisecond() < other.getMillisecond();
}

bool Timestamp::millisecondEqual(const Timestamp &other) const
{
    return (second_ == other.second_ &&
            (getMillisecond() == other.getMillisecond()));
}

int64_t Timestamp::distanceSecond(const Timestamp &other) const
{
    if (second_ > other.second_) {
        return second_ - other.second_;
    }
    return other.second_ - second_;
}

int64_t Timestamp::distanceMillisecond(const Timestamp &other) const
{
    const Timestamp *bigger = nullptr;
    const Timestamp *smaller = nullptr;

    if (*this < other) {
        bigger = &other;
        smaller = this;
    } else {
        bigger = this;
        smaller = &other;
    }

    return (bigger->second_ - smaller->second_) * 1000 +
           (bigger->getMillisecond() - smaller->getMillisecond());
}

time_t Timestamp::now()
{
    struct timespec tv;
    ::clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec;
}

size_t Timestamp::format(char *buffer, size_t size, const char *format, time_t second)
{
    struct tm tm;
    ::localtime_r(&second, &tm);
    return strftime(buffer, size, format, &tm);
}


} // namespace base
} // namespace yut
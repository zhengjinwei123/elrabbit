#ifndef ELE_BASE_NONCOPYABLE_H
#define ELE_BASE_NONCOPYABLE_H

namespace ele
{

    class noncopyable
    {
    public:
        noncopyable(const noncopyable &) = delete;
        void operator=(const noncopyable &) = delete;

    protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };
} // namespace ele

#endif
#ifndef ELRABBIT_COMMON_ATOMIC_H
#define ELRABBIT_COMMON_ATOMIC_H

#include <stdint.h>
#include "elrabbit/common/class_util.h"

namespace elrabbit {

namespace common {

template<typename T>
class AtomicIntegerT : Noncopyable
{
public:
    AtomicIntegerT() :
        value_(0) {}
    T get()
    {
        return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
        // return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndAdd(T x)
    {
        return  __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST);
        // return __sync_fetch_add_add(&value_, x);
    }

    T getAndSet(T new_value)
    {
        return __atomic_exchange_n(&value_, new_value, __ATOMIC_SEQ_CST);
        // return __sync_lock_test_and_set(&value_, new_value);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        decrementAndGet();
    }


private:
    volatile T value_; // 防止编译器优化， 每次直接从内存读取
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

} // namespace common
} // namespace elrabbit

#endif // ELRABBIT_COMMON_ATOMIC_H

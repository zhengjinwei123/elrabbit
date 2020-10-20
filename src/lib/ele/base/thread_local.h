#ifndef ELE_BASE_THREAD_LOCAL_H
#define ELE_BASE_THREAD_LOCAL_H

#include <pthread.h>
#include <thread>
#include "noncopyable.h"


namespace ele
{
    template <class T>
    class ThreadLocal : noncopyable
    {
    public:
        ThreadLocal()
        {
            ::pthread_key_create(&pkey_, &ThreadLocal::destructor);
        }

        ~ThreadLocal()
        {
            ::pthread_key_delete(pkey_);
        }

        T &value()
        {
            T *per_thread_value = static_cast<T *>(pthread_getspecific(pkey_));
            if (!per_thread_value)
            {
                T *obj = new T();
                pthread_setspecific(pkey_, obj);
                per_thread_value = obj;
            }
            return *per_thread_value;
        }

    private:
        pthread_key_t pkey_;

    private:
        static void destructor(void *x)
        {
            T *obj = static_cast<T *>(x);
            delete obj;
        }
    }
} // namespace ele
#endif
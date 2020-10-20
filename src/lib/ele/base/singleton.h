#ifndef ELE_BASE_SINGLETON_H
#define ELE_BASE_SINGLETON_H

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <pthread.h>

#include "noncopyable.h"

namespace ele
{

    template <class T>
    struct has_no_destroy
    {
        template <class C>
        static char test(decltype(&C::no_destroy));

        template <class C>
        static int32_t test(...);

        const static bool value = sizeof(test<T>(0)) == 1;
    };

    template <typename T>
    class Singleton : noncopyable
    {
    private:
        static pthread_once_t ponce_;
        static T *value_;

    public:
        static T &instance()
        {
            pthread_once(&ponce_, &Singleton::init);
            assert(value_ != nullptr);

            return *value_;
        }

    private:
        Singleton() = default;
        ~Singleton() = default;

        static void init()
        {
            value_ = new T();
            if (!has_no_destroy<T>::value)
            {
                ::atexit(destroy);
            }
        }

        static void destroy()
        {
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void)(dummy);

            delete value_;
            value_ = nullptr;
        }
    };

    template <typename T>
    pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

    template <typename T>
    T *Singleton<T>::value_ = nullptr;

} // namespace ele

#endif // ELE_BASE_SINGLETON_H
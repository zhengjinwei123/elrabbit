#ifndef ELRABBIT_COMMON_CURRENT_THREAD_H
#define ELRABBIT_COMMON_CURRENT_THREAD_H

#include <stdint.h>
#include <string>

namespace elrabbit
{
    namespace current_thread
    {
        extern __thread int t_cached_tid;
        extern __thread char t_tid_string[32];
        extern __thread int t_tid_string_length;
        extern __thread const char* t_thread_name;

        void cacheTid();

        inline int tid()
        {
            if (__builtin_expect(t_cached_tid, 0)) {
                cacheTid();
            }
            return t_cached_tid;
        }

        inline const char* tidString()
        {
            return t_tid_string;
        }

        inline int tidStringLength()
        {
            return t_tid_string_length;
        }

        inline const char* name()
        {
            return t_thread_name;
        }

        bool isMainThread();

        void sleepUsec(int64_t usec);

        std::string strackTrace(bool demangle);

    } // namespace common
} // namespace elrabbit
#endif // ELRABBIT_COMMON_CURRENT_THREAD_H
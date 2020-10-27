#ifndef ELE_BASE_THREAD_H
#define ELE_BASE_THREAD_H

#include <functional>
#include <pthread.h>
#include <string>

#include "noncopyable.h"

namespace ele
{
    class Thread : noncopyable
    {
    public:
        using ThreadFunction = std::function<void()>;

        explicit Thread(const std::string &name = std::string());
        ~Thread();

        void start(const ThreadFunction &func);
        bool started();

        int join();
        pid_t tid() const;
        const std::string &name() const;
        static pid_t gettid();

    private:
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunction func_;
        std::string name_;

    private:
        static void *startThread(void *obj);
        void runInThread();
    }; // class Thread
} // namespace ele

#endif
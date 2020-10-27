#include "thread.h"

#include <cassert>
#include <cerrno>
#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

#include <exception>

namespace ele
{
    Thread::Thread(const std::string &name) : started_(false),
                                              joined_(false),
                                              pthreadId_(0),
                                              tid_(0),
                                              name_(name)
    {
    }

    Thread::~Thread()
    {
        if (started_ && !joined_)
        {
            pthread_detach(pthreadId_);
        }
    }

    void Thread::start(const ThreadFunction &func)
    {
        assert(!started_);
        func_ = func;
        started_ = true;
        errno = pthread_create(&pthreadId_, nullptr, &startThread, this);
        if (errno != 0)
        {
        }
    }

    bool Thread::started()
    {
        return started_;
    }

    int Thread::join()
    {
        assert(started_);
        assert(!joined_);

        joined_ = true;

        return pthread_join(pthreadId_, nullptr);
    }

    pid_t Thread::tid() const
    {
        return tid_;
    }

    const std::string &Thread::name() const
    {
        return name_;
    }

    pid_t Thread::gettid()
    {
        return static_cast<pid_t>(::syscall(__NR_gettid));
    }

    void *Thread::startThread(void *obj)
    {
        auto *thread = static_cast<Thread *>(obj);
        thread->runInThread();
        return nullptr;
    }

    void Thread::runInThread()
    {
        tid_ = Thread::gettid();
        try
        {
            func_();
        }
        catch (const std::exception &ex)
        {
            abort();
        }
        catch (...)
        {
            throw;
        }
    }

} // namespace ele

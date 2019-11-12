#include "elrabbit/common/thread.h"
#include "elrabbit/common/current_thread.h"
#include "elrabbit/common/exception.h"
#include "elrabbit/common/timestamp.h"
#include "elrabbit/common/atomic.h"

#include <type_traits>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <assert.h>

namespace elrabbit {

namespace detail {
    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void afterFork()
    {
        current_thread::t_cached_tid = 0;
        current_thread::t_thread_name = "main";
        current_thread::tid();
    }

    class ThreadNameInitializer
    {
    public:
        ThreadNameInitializer()
        {
            current_thread::t_thread_name = "main";
            current_thread::tid();
            pthread_atfork(nullptr, nullptr, &afterFork);
        }
    };

    ThreadNameInitializer init;

    struct ThreadData {
        typedef common::Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        std::string name_;
        pid_t* tid_;
        common::CountDownLatch* latch_;

        ThreadData(ThreadFunc func,
                const std::string& name,
                pid_t* tid,
                common::CountDownLatch* latch) :
            func_(std::move(func)),
            name_(name),
            tid_(tid),
            latch_(latch)
        {

        }

        void runInThread()
        {
            *tid_ = current_thread::tid();
            tid_ = nullptr;
            latch_->countDown();
            latch_ = nullptr;

            current_thread::t_thread_name = name_.empty() ? "elrabbit_thread" : name_.c_str();
            ::prctl(PR_SET_NAME, current_thread::t_thread_name);

            try {
                func_();
                current_thread::t_thread_name = "finished";
            } catch (const common::Exception& ex)
            {
                current_thread::t_thread_name = "crashed";
                fprintf(stderr, "exception caught in thread %s\n", name_.c_str());
                fprintf(stderr, "reason:%s\n", ex.what());
                fprintf(stderr, "stack strace: %s\n", ex.stackTrace());
                abort();
            }
            catch (const std::exception& ex)
            {
                current_thread::t_thread_name = "crashed";
                fprintf(stderr, "exception caught in thread %s\n", name_.c_str());
                fprintf(stderr, "reason:%s\n", ex.what());
                abort();
            }
            catch(...) 
            {
                current_thread::t_thread_name = "crashed";
                fprintf(stderr, "exception caught in thread %s\n", name_.c_str());
                throw;
            }
        }
    };

    void* startThread(void* obj)
    {
        ThreadData* data = static_cast<ThreadData*>(obj);
        data->runInThread();
        delete data;
        return nullptr;
    }
} // namespace detail


void current_thread::cacheTid()
{
    if (current_thread::t_cached_tid == 0) {
        current_thread::t_cached_tid = detail::gettid();
        current_thread::t_tid_string_length = snprintf(current_thread::t_tid_string, sizeof(current_thread::t_tid_string), "%5d", current_thread::t_cached_tid);
    }
}

bool current_thread::isMainThread()
{
    return current_thread::tid() == ::getpid();
}

void current_thread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / common::Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % common::Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, nullptr);
}

namespace common {

AtomicInt32 Thread::num_created_;

Thread::Thread(ThreadFunc func, const std::string& n) :
    started_(false),
    joined_(false),
    pthread_id_(0),
    tid_(0),
    func_(std::move(func)),
    name_(n),
    latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_) {
        pthread_detach(pthread_id_);
    }
}

void Thread::setDefaultName()
{
    int num = num_created_.incrementAndGet();
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthread_id_, nullptr, &detail::startThread, data)) {
        started_ = false;
        delete data;
    }
    else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthread_id_, nullptr);
}

} // namespace common
} // namespace elrabbit

#ifndef ELRABBIT_COMMON_THREAD_H
#define ELRABBIT_COMMON_THREAD_H

#include <functional>
#include <memory>
#include <pthread.h>
#include <string>

#include "elrabbit/common/class_util.h"
#include "elrabbit/common/atomic.h"
#include "elrabbit/common/countdown_latch.h"

namespace elrabbit {

namespace common {

class Thread : Noncopyable {
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string& n);

    ~Thread();

    void start();
    int join();

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated() { return num_created_.get(); }

private:
    void setDefaultName();
    bool started_;
    bool joined_;
    pthread_t pthread_id_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    static AtomicInt32 num_created_;
};

} // namespace common
}// namespace elrabbit

#endif // ELRABBIT_COMMON_THREAD_H
#ifndef ELE_COMMON_ASYNC_TASK_H
#define ELE_COMMON_ASYNC_TASK_H

#include <functional>
#include <lib/ele/base/class_util.h>

namespace ele
{
    namespace common
    {
        class AsyncTask
        {
            public:
                using TaskCallback = std::function<void (AsyncTask *)>;

                AsyncTask();
                virtual ~AsyncTask() = 0;

                void doCallback();
                const TaskCallback &getTaskCallback() const { return task_cb_; }
                void setTaskCallback(const TaskCallback &task_cb);

            private:
                ELE_NONCOPYABLE(AsyncTask);

                TaskCallback task_cb_;
        };
    } // namespace common
} // namespace ele
#endif
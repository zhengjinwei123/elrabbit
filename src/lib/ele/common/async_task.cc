#include "async_task.h"

namespace ele
{
    namespace common
    {
        AsyncTask::AsyncTask()
        {
        }

        AsyncTask::~AsyncTask()
        {
        }

        void AsyncTask::doCallback()
        {
            if (task_cb_)
            {
                task_cb_(this);
            }
        }

        void AsyncTask::setTaskCallback(const TaskCallback &task_cb)
        {
            task_cb_ = task_cb;
        }

    } // namespace common
} // namespace ele
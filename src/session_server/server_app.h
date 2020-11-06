#ifndef SESSION_SERVER_SERVER_APP_H
#define SESSION_SERVER_SERVER_APP_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>
#include <stdint.h>

namespace ele
{
    namespace common
    {
        class NetCommand;
        class AsyncTask;
    } // namespace common
} // namespace ele

namespace session_server
{
    class ServerApp
    {
    public:
        using TimerCallback = std::function<void(int64_t)>;

        bool init();
        void loop();
        void finalize();

        int64_t startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times = 1);
        void stopTimer(int64_t timer_id);

        void pushSignal(int signum);
        void pushAsyncTask(std::unique_ptr<ele::common::AsyncTask> &task);
        void pushClientNetCommand(std::unique_ptr<ele::common::NetCommand> &cmd);

    private:
        ELE_SINGLETON(ServerApp);

        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };
} // namespace session_server

#define sServerApp session_server::ServerApp::getInstance()

#endif
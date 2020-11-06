#include "session_server/server_app.h"

#include <csignal>
#include <cstddef>
#include <lib/ele/net/io_service.h>
#include <lib/ele/net/message_queue.h>
#include <lib/ele/net/signal_queue.h>

#include "session_server/client_net_manager.h"
#include <lib/ele/common/async_task.h>
#include <lib/ele/common/log.h>
#include <lib/ele/common/net_command.h>

namespace session_server
{
    using ele::common::AsyncTask;
    using ele::common::NetCommand;
    using ele::net::IOService;
    using ele::net::MessageQueue;
    using ele::net::SignalQueue;

    class ServerApp::Impl
    {
    public:
        using TimerCallback = ServerApp::TimerCallback;
        using AsyncTaskQueue = MessageQueue<AsyncTask *>;
        using NetCommandQueue = MessageQueue<NetCommand *>;

        Impl();
        ~Impl();

        bool init();
        void loop();
        void finalize();

        int64_t startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times);
        void stopTimer(int64_t timer_id);

        void pushSignal(int signum);
        void pushAsyncTask(std::unique_ptr<AsyncTask> &task);
        void pushClientNetCommand(std::unique_ptr<NetCommand> &cmd);

    private:
        void onSignal(int signum);
        void onAsyncTask(AsyncTaskQueue *queue);
        void onClientNetCommand(NetCommandQueue *queue);

    private:
        IOService io_service_;
        SignalQueue signal_queue_;
        AsyncTaskQueue async_task_queue_;
        NetCommandQueue client_net_command_queue_;
    };

    ServerApp::Impl::Impl() : signal_queue_(io_service_),
                              async_task_queue_(io_service_),
                              client_net_command_queue_(io_service_)
    {
    }

    ServerApp::Impl::~Impl()
    {
    }

    bool ServerApp::Impl::init()
    {
        signal_queue_.setRecvSignalCallback(std::bind(&ServerApp::Impl::onSignal, this, std::placeholders::_1));
        async_task_queue_.setRecvMessageCallback(std::bind(&ServerApp::Impl::onAsyncTask, this, std::placeholders::_1));
        client_net_command_queue_.setRecvMessageCallback(std::bind(&ServerApp::Impl::onClientNetCommand, this, std::placeholders::_1));

        ClientNetManager::getInstance();

        if (sClientNetManager->init() == false)
        {
            LOG_ERROR("init client_net failed");
            return false;
        }

        return true;
    }

    void ServerApp::Impl::loop()
    {
        io_service_.loop();
    }

    void ServerApp::Impl::finalize()
    {
    }

    int64_t ServerApp::Impl::startTimer(int64_t timeout_ms,
                                        const TimerCallback &timer_cb, int call_times)
    {
        return io_service_.startTimer(timeout_ms, timer_cb, call_times);
    }

    void ServerApp::Impl::stopTimer(int64_t timer_id)
    {
        io_service_.stopTimer(timer_id);
    }

    void ServerApp::Impl::pushSignal(int signum)
    {
        signal_queue_.push(signum);
    }

    void ServerApp::Impl::pushAsyncTask(std::unique_ptr<AsyncTask> &task)
    {
        async_task_queue_.push(task.get());
        task.release();
    }

    void ServerApp::Impl::pushClientNetCommand(std::unique_ptr<NetCommand> &cmd)
    {
        client_net_command_queue_.push(cmd.get());
        cmd.release();
    }

    void ServerApp::Impl::onSignal(int signum)
    {
        if (SIGTERM == signum || SIGINT == signum)
        {
            LOG_INFO("receive quit signal");
            io_service_.quit();
        }
    }

    void ServerApp::Impl::onAsyncTask(AsyncTaskQueue *queue)
    {
        AsyncTask *task_raw = nullptr;

        while (queue->pop(task_raw))
        {
            std::unique_ptr<AsyncTask> task(task_raw);
            task->doCallback();
        }
    }

    void ServerApp::Impl::onClientNetCommand(NetCommandQueue *queue)
    {
        ele::common::NetCommand *cmd_raw = nullptr;
        size_t count = 0;

        while (queue->pop(cmd_raw))
        {
            std::unique_ptr<NetCommand> cmd(cmd_raw);
            sClientNetManager->processNetCommand(cmd_raw);

            // 防止饥饿
            if (++count >= 100)
            {
                queue->yield();
                return;
            }
        }
    }

    ServerApp::ServerApp() : pimpl_(new Impl())
    {
    }

    ServerApp::~ServerApp()
    {
    }

    bool ServerApp::init()
    {
        return pimpl_->init();
    }

    void ServerApp::loop()
    {
        pimpl_->loop();
    }

    void ServerApp::finalize()
    {
        pimpl_->finalize();
    }

    int64_t ServerApp::startTimer(int64_t timeout_ms, const TimerCallback &timer_cb, int call_times)
    {
        return pimpl_->startTimer(timeout_ms, timer_cb, call_times);
    }

    void ServerApp::stopTimer(int64_t timer_id)
    {
        pimpl_->stopTimer(timer_id);
    }

    void ServerApp::pushSignal(int signum)
    {
        pimpl_->pushSignal(signum);
    }

    void ServerApp::pushAsyncTask(std::unique_ptr<AsyncTask> &task)
    {
        pimpl_->pushAsyncTask(task);
    }

    void ServerApp::pushClientNetCommand(std::unique_ptr<NetCommand> &cmd)
    {
        pimpl_->pushClientNetCommand(cmd);
    }

} // namespace session_server
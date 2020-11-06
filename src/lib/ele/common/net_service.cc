#include "net_service.h"

#include <cstring>
#include <ext/hash_map>
#include <ext/hash_set>
#include <lib/ele/base/dynamic_buffer.h>
#include <lib/ele/base/thread.h>
#include <lib/ele/base/timestamp.h>
#include <lib/ele/exchange/base_struct.h>
#include <lib/ele/net/io_service.h>
#include <lib/ele/net/message_queue.h>
#include <lib/ele/net/socket_addr.h>
#include <lib/ele/net/tcp_service.h>
#include <lib/ele/net/tcp_socket.h>
#include <vector>

#include "log.h"
#include "net_command.h"
#include "net_id.h"
#include "net_protocol.h"

namespace ele
{
    namespace common
    {
        using ele::exchange::BaseStruct;
        using ele::net::IOService;
        using ele::net::MessageQueue;
        using ele::net::SocketAddr;
        using ele::net::TcpService;
        using ele::net::TcpSocket;

        namespace net_service_impl
        {
            class NetThread
            {
            public:
                using NewNetCommandCallback = NetService::NewNetCommandCallback;
                using CreateMessageFunc = NetService::CreateMessageFunc;
                using NetCommandQueue = MessageQueue<NetCommand *>;
                using BroadcastList = __gnu_cxx::hash_set<TcpService::SocketId>;

                NetThread(int max_recv_buffer_size,
                          int max_recv_packet_length,
                          int max_send_buffer_size,
                          int max_send_packet_length,
                          const CreateMessageFunc &create_message_func);
                ~NetThread();

                bool init(const std::string &service_name, int id,
                          const TcpSocket &listen_socket, const NewNetCommandCallback &new_net_cmd_cb);
                void start();
                void join();
                void push(NetCommand *cmd);

            private:
                void sendNetCommandNew(TcpService::SocketId socket_id);
                void sendNetCommandClose(TcpService::SocketId socket_id);
                void closeSocket(TcpService::SocketId socket_id);

                void onNewConnection(TcpService *service,
                                     TcpService::SocketId from_socket_id,
                                     TcpService::SocketId socket_id);
                void onRecvMessage(TcpService *service,
                                   TcpService::SocketId socket_id,
                                   DynamicBuffer *buffer);
                void onPeerClose(TcpService *service,
                                 TcpService::SocketId socket_id);
                void onError(TcpService *service, TcpService::SocketId socket_id, int error);
                void onNetCommand(NetCommandQueue *queue);

            private:
                std::string service_name_;
                int id_;
                Thread thread_;
                IOService io_service_;
                TcpService tcp_service_;
                NetCommandQueue command_queue_;

                NewNetCommandCallback new_net_cmd_cb_;
                NetProtocol net_protocol_;
                DynamicBuffer encode_buffer_;
                BroadcastList broadcast_list_;
            };

            NetThread::NetThread(int max_recv_buffer_size,
                                 int max_recv_packet_length,
                                 int max_send_buffer_size,
                                 int max_send_packet_length,
                                 const CreateMessageFunc &create_message_func) : id_(-1),
                                                                                 tcp_service_(io_service_),
                                                                                 command_queue_(io_service_),
                                                                                 net_protocol_(max_recv_packet_length, create_message_func),
                                                                                 encode_buffer_(max_send_packet_length)
            {
                tcp_service_.setRecvBufferMaxSize(max_recv_buffer_size);
                tcp_service_.setSendBufferMaxSize(max_send_buffer_size);
                tcp_service_.setAcceptPauseTimeWhenExceedOpenFileLimit(2000);
            }

            NetThread::~NetThread()
            {
            }

            bool NetThread::init(const std::string &service_name, int id,
                                 const TcpSocket &listen_socket, const NewNetCommandCallback &new_net_cmd_cb)
            {
                service_name_ = service_name;
                id_ = id;
                new_net_cmd_cb_ = new_net_cmd_cb;

                tcp_service_.setNewConnectionCallback(std::bind(&NetThread::onNewConnection,
                                                                this,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2,
                                                                std::placeholders::_3));
                tcp_service_.setRecvMessageCallback(std::bind(&NetThread::onRecvMessage,
                                                              this,
                                                              std::placeholders::_1,
                                                              std::placeholders::_2,
                                                              std::placeholders::_3));
                tcp_service_.setPeerCloseCallback(std::bind(&NetThread::onPeerClose,
                                                            this,
                                                            std::placeholders::_1,
                                                            std::placeholders::_2));
                tcp_service_.setErrorCallback(std::bind(&NetThread::onError, this,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2,
                                                        std::placeholders::_3));
                command_queue_.setRecvMessageCallback(std::bind(&NetThread::onNetCommand, this,
                                                                std::placeholders::_1));

                if (tcp_service_.shareListen(listen_socket) < 0)
                {
                    LOG_ERROR("%s thread(%d) share listen failed", service_name_.c_str(), id_);
                    return false;
                }
                return true;
            }

            void NetThread::start()
            {
                thread_.start(std::bind(&IOService::loop, &io_service_));
            }

            void NetThread::join()
            {
                thread_.join();
            }

            void NetThread::push(NetCommand *cmd)
            {
                command_queue_.push(cmd);
            }

            void NetThread::sendNetCommandNew(TcpService::SocketId socket_id)
            {
                std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::NEW));
                cmd->id.service_id = id_;
                cmd->id.socket_id = socket_id;
                new_net_cmd_cb_(cmd);
            }

            void NetThread::sendNetCommandClose(TcpService::SocketId socket_id)
            {
                std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::CLOSE));
                cmd->id.service_id = id_;
                cmd->id.socket_id = socket_id;
                new_net_cmd_cb_(cmd);
            }

            void NetThread::closeSocket(TcpService::SocketId socket_id)
            {
                sendNetCommandClose(socket_id);
                tcp_service_.closeSocket(socket_id);
                broadcast_list_.erase(socket_id);
            }

            void NetThread::onNewConnection(TcpService *service,
                                            TcpService::SocketId from_socket_id,
                                            TcpService::SocketId socket_id)
            {
                SocketAddr remote_addr;
                service->getPeerAddr(socket_id, &remote_addr);

                LOG_DEBUG("%s new connection on net_id (%d:%lx), addr=[%s:%d]",
                          service_name_.c_str(), id_, socket_id, remote_addr.getIp().c_str(),
                          remote_addr.getPort());

                sendNetCommandNew(socket_id);
            }

            void NetThread::onRecvMessage(TcpService *service,
                                          TcpService::SocketId socket_id,
                                          DynamicBuffer *buffer)
            {
                for (;;)
                {
                    int message_id = 0;
                    std::unique_ptr<BaseStruct> message;

                    NetProtocol::RetCode::type ret =
                        net_protocol_.recvMessage(buffer, &message_id, message);
                    if (NetProtocol::RetCode::WAITING_MORE_DATA == ret)
                    {
                        return;
                    }
                    else if (NetProtocol::RetCode::ERROR == ret)
                    {
                        LOG_ERROR("%s recv message failed on net_id (%d:%lx)",
                                  service_name_.c_str(), id_, socket_id);
                        closeSocket(socket_id);
                        return;
                    }
                    else if (NetProtocol::RetCode::MESSAGE_READY == ret)
                    {
                        std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
                        cmd->id.service_id = id_;
                        cmd->id.socket_id = socket_id;
                        cmd->message_id = message_id;
                        cmd->message = message.get();
                        new_net_cmd_cb_(cmd);
                    }
                }
            }

            void NetThread::onPeerClose(TcpService *service,
                                        TcpService::SocketId socket_id)
            {
                LOG_DEBUG("%s peer close on net_id(%d:%lx)",
                          service_name_.c_str(), id_, socket_id);
                closeSocket(socket_id);
            }

            void NetThread::onError(TcpService *service, TcpService::SocketId socket_id, int error)
            {
                LOG_ERROR("%s error occur on net_id(%d:%lx), error_code=[%d], error=[%s]",
                          service_name_.c_str(),
                          id_,
                          socket_id,
                          error,
                          strerror(error));
                closeSocket(socket_id);
            }

            void NetThread::onNetCommand(NetCommandQueue *queue)
            {
                NetCommand *cmd_raw = nullptr;

                while (queue->pop(cmd_raw))
                {
                    if (nullptr == cmd_raw)
                    {
                        io_service_.quit();
                        return;
                    }

                    std::unique_ptr<NetCommand> cmd(cmd_raw);

                    if (NetCommand::Type::BROADCAST == cmd->type)
                    {
                        if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false)
                        {
                            LOG_DEBUG("%s encode broadcast message failed on net_thread(%d)", service_name_.c_str(), id_);
                            continue;
                        }

                        for (BroadcastList::const_iterator iter = broadcast_list_.begin();
                             iter != broadcast_list_.end(); ++iter)
                        {
                            tcp_service_.sendMessage(*iter,
                                                     encode_buffer_.readBegin(),
                                                     encode_buffer_.readableBytes());
                        }
                        continue;
                    }

                    if (tcp_service_.isConnected(cmd->id.socket_id) == false)
                    {
                        continue;
                    }

                    if (NetCommand::Type::CLOSE == cmd->type)
                    {
                        tcp_service_.closeSocket(cmd->id.service_id);
                        continue;
                    }
                    else if (NetCommand::Type::MESSAGE == cmd->type)
                    {
                        if (net_protocol_.writeMessage(cmd->message_id, cmd->message, &encode_buffer_) == false)
                        {
                            LOG_ERROR("%s encode message(%d) failed on net_id(%d:%lx)",
                                      service_name_.c_str(),
                                      cmd->message_id,
                                      id_,
                                      cmd->id.socket_id);
                            closeSocket(cmd->id.service_id);
                            continue;
                        }

                        tcp_service_.sendMessage(cmd->id.service_id,
                                                 encode_buffer_.readBegin(), encode_buffer_.readableBytes());
                    }
                    else if (NetCommand::Type::ENABLE_BROADCAST == cmd->type)
                    {
                        broadcast_list_.insert(cmd->id.socket_id);
                    }
                    else if (NetCommand::Type::DISABLE_BROADCAST == cmd->type)
                    {
                        broadcast_list_.erase(cmd->id.socket_id);
                    }
                }
            }

        } // namespace net_service_impl

        using namespace net_service_impl;

        class NetService::Impl
        {
        public:
            struct ConnectionInfo
            {
                time_t last_request_second;
                int request_per_second;
            };

            using NewNetCommandCallback = NetService::NewNetCommandCallback;
            using CreateMessageFunc = NetService::CreateMessageFunc;
            using MessageHandler = NetService::MessageHandler;
            using NetThreadVector = std::vector<NetThread *>;
            using MessageHandlerMap = __gnu_cxx::hash_map<int, MessageHandler>;
            using ConnectionInfoMap = __gnu_cxx::hash_map<NetId, ConnectionInfo, NetId::Hash>;

            Impl(NetService *thiz);
            ~Impl();

            bool init(const std::string &service_name, int thread_count,
                const std::string &ip, uint16_t port,
                const NewNetCommandCallback &new_net_cmd_cb,
                const CreateMessageFunc &create_message_func,
                int max_recv_buffer_size,
                int max_recv_packet_length,
                int max_send_buffer_size,
                int max_send_packet_length,
                int max_request_per_second);

            void start();
            void stop();

            MessageHandler getMessageHandler(int message_id) const;
            void setMessageHandler(int message_id, const MessageHandler &message_handler);

            void disconnect(const NetId &net_id);
            void sendMessage(const NetId &net_id, int message_id, std::unique_ptr<BaseStruct> &message);
            void enableBroadcast(const NetId &net_id);
            void disableBroadcast(const NetId &net_id);
            void broadcastMessage(int message_id, std::unique_ptr<BaseStruct> &message);

            void processNetCommand(const NetCommand *cmd);

        private:
            void push(std::unique_ptr<NetCommand> &cmd);

            void onConnect(const NetId &net_id);
            void onDisconnect(const NetId &net_id);
            void onMessage(const NetId &net_id, int message_id, const BaseStruct *message);
            bool checkRequestFrequencyLimit(ConnectionInfo &info);

        private:
            NetService *thiz_;
            std::string service_name_;
            TcpSocket listen_socket_;
            NetThreadVector threads_;
            MessageHandlerMap message_handlers_;

            ConnectionInfoMap connection_infos_;
            int max_request_per_second_;
        };

        NetService::Impl::Impl(NetService *thiz) : thiz_(thiz),
                                                   max_request_per_second_(0)
        {
        }

        NetService::Impl::~Impl()
        {
            for (size_t i = 0; i < threads_.size(); ++i)
            {
                delete threads_[i];
            }
        }

        bool NetService::Impl::init(const std::string &service_name, int thread_count,
                                    const std::string &ip, uint16_t port,
                                    const NewNetCommandCallback &new_net_cmd_cb,
                                    const CreateMessageFunc &create_message_func,
                                    int max_recv_buffer_size,
                                    int max_recv_packet_length,
                                    int max_send_buffer_size,
                                    int max_send_packet_length,
                                    int max_request_per_second)
        {
            service_name_ = service_name;
            max_request_per_second_ = max_request_per_second;

            SocketAddr addr(ip, port);

            if (listen_socket_.passiveOpenNonblock(addr) == false)
            {
                LOG_ERROR("%s listen on %s:%d failed", service_name.c_str(),
                          addr.getIp().c_str(), addr.getPort());
                return false;
            }

            LOG_INFO("%s listen on %s:%d", service_name.c_str(), addr.getIp().c_str(), addr.getPort());

            threads_.resize(thread_count, nullptr);

            for (size_t i = 0; i < threads_.size(); ++i)
            {
                threads_[i] = new NetThread(max_recv_buffer_size,
                                            max_recv_packet_length,
                                            max_send_buffer_size,
                                            max_send_packet_length,
                                            create_message_func);
            }

            for (size_t i = 0; i < threads_.size(); ++i)
            {
                if (threads_[i]->init(service_name, i, listen_socket_, new_net_cmd_cb) == false)
                {
                    LOG_ERROR("%s init net thread(%d) failed", service_name.c_str(), i);
                    return false;
                }
            }

            return true;
        }

        void NetService::Impl::start()
        {
            for (size_t i = 0; i < threads_.size(); ++i)
            {
                threads_[i]->start();
            }
        }

        void NetService::Impl::stop()
        {
            for (size_t i = 0; i < threads_.size(); ++i)
            {
                threads_[i]->push(nullptr);
            }

            for (size_t i = 0; i < threads_.size(); ++i)
            {
                threads_[i]->join();
            }
        }

        void NetService::Impl::push(std::unique_ptr<NetCommand> &cmd)
        {
            threads_[cmd->id.service_id]->push(cmd.get());
            cmd.release();
        }

        NetService::Impl::MessageHandler NetService::Impl::getMessageHandler(int message_id) const
        {
            MessageHandlerMap::const_iterator iter = message_handlers_.find(message_id);
            if (iter == message_handlers_.end())
            {
                return nullptr;
            }

            return iter->second;
        }

        void NetService::Impl::setMessageHandler(int message_id, const MessageHandler &message_handler)
        {
            message_handlers_[message_id] = message_handler;
        }

        void NetService::Impl::disconnect(const NetId &net_id)
        {
            std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::CLOSE));
            cmd->id = net_id;
            push(cmd);

            onDisconnect(net_id);
        }

        void NetService::Impl::sendMessage(const NetId &net_id, int message_id, std::unique_ptr<BaseStruct> &message)
        {
            std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::MESSAGE));
            cmd->id = net_id;
            cmd->message_id = message_id;

            cmd->message = message.release();
            push(cmd);
        }

        void NetService::Impl::enableBroadcast(const NetId &net_id)
        {
            std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::ENABLE_BROADCAST));
            cmd->id = net_id;
            push(cmd);
        }

        void NetService::Impl::disableBroadcast(const NetId &net_id)
        {
            std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::DISABLE_BROADCAST));
            cmd->id = net_id;
            push(cmd);
        }

        void NetService::Impl::broadcastMessage(int message_id, std::unique_ptr<BaseStruct> &message)
        {
            for (size_t i = 0; i < threads_.size(); ++i)
            {
                std::unique_ptr<NetCommand> cmd(new NetCommand(NetCommand::Type::BROADCAST));
                cmd->id.service_id = (int)i;
                cmd->message_id = message_id;
                cmd->message = message->clone();
                push(cmd);
            }

            message.reset();
        }

        void NetService::Impl::processNetCommand(const NetCommand *cmd)
        {
            if (NetCommand::Type::NEW == cmd->type)
            {
                onConnect(cmd->id);
            }
            else if (NetCommand::Type::CLOSE == cmd->type)
            {
                onDisconnect(cmd->id);
            }
            else if (NetCommand::Type::MESSAGE == cmd->type)
            {
                onMessage(cmd->id, cmd->message_id, cmd->message);
            }
        }

        void NetService::Impl::onConnect(const NetId &net_id)
        {
            ConnectionInfo info;
            info.last_request_second = 0;
            info.request_per_second = 0;
            connection_infos_.insert(std::make_pair(net_id, info));

            thiz_->onConnect(net_id);
        }

        void NetService::Impl::onDisconnect(const NetId &net_id)
        {
            NetId net_id_copy = net_id;
            thiz_->onDisconnect(net_id_copy);

            connection_infos_.erase(net_id_copy);
        }

        void NetService::Impl::onMessage(const NetId &net_id,
                                         int message_id, const BaseStruct *message)
        {
            ConnectionInfoMap::iterator iter = connection_infos_.find(net_id);
            if (iter == connection_infos_.end())
            {
                return;
            }

            ConnectionInfo &info = iter->second;

            if (max_request_per_second_ > 0 &&
                checkRequestFrequencyLimit(info))
            {
                LOG_ERROR("net_id(%d:%lx) exceed request frequency limit(%zd >= %d)",
                          net_id.service_id, net_id.socket_id,
                          info.request_per_second,
                          max_request_per_second_);
                thiz_->onExceedRequestFrequencyLimit(net_id);
                disconnect(net_id);
                return;
            }

            MessageHandler func = getMessageHandler(message_id);
            if (func)
            {
                func(net_id, message);
            }
            else
            {
                LOG_WARNING("%s message_id(%d) handler not set",
                            service_name_.c_str(), message_id);
            }
        }

        bool NetService::Impl::checkRequestFrequencyLimit(ConnectionInfo &info)
        {
            time_t now = Timestamp::now();

            if (now == info.last_request_second)
            {
                ++info.request_per_second;
            }
            else
            {
                info.last_request_second = now;
                info.request_per_second = 0;
            }

            if (info.request_per_second > max_request_per_second_)
            {
                return true;
            }

            return false;
        }

        NetService::NetService() : pimpl_(new Impl(this))
        {
        }

        NetService::~NetService()
        {
        }

        bool NetService::init(const std::string &service_name, int thread_count,
                              const std::string &ip, uint16_t port,
                              const NewNetCommandCallback &new_net_cmd_cb,
                              const CreateMessageFunc &create_message_func,
                              int max_recv_buffer_size,
                              int max_recv_packet_length,
                              int max_send_buffer_size,
                              int max_send_packet_length,
                              int max_request_per_second)
        {
            return pimpl_->init(service_name, thread_count, ip, port,
                                new_net_cmd_cb, create_message_func, max_recv_buffer_size,
                                max_recv_packet_length,
                                max_send_buffer_size,
                                max_send_packet_length,
                                max_request_per_second);
        }

        void NetService::start()
        {
            pimpl_->start();
        }

        void NetService::stop()
        {
            pimpl_->stop();
        }

        NetService::MessageHandler NetService::getMessageHandler(int message_id) const
        {
            return pimpl_->getMessageHandler(message_id);
        }

        void NetService::setMessageHandler(int message_id,
                                           const MessageHandler &message_handler)
        {
            pimpl_->setMessageHandler(message_id, message_handler);
        }

        void NetService::disconnect(const NetId &net_id)
        {
            if (net_id.isInValid())
            {
                return;
            }

            pimpl_->disconnect(net_id);
        }

        void NetService::sendMessage(const NetId &net_id, int message_id, std::unique_ptr<BaseStruct> &message)
        {
            if (net_id.isInValid())
            {
                return;
            }
            pimpl_->sendMessage(net_id, message_id, message);
        }

        void NetService::enableBroadcast(const NetId &net_id)
        {
            if (net_id.isInValid())
            {
                return;
            }

            pimpl_->enableBroadcast(net_id);
        }

        void NetService::disableBroadcast(const NetId &net_id)
        {
            if (net_id.isInValid())
            {
                return;
            }

            pimpl_->disableBroadcast(net_id);
        }

        void NetService::broadcastMessage(int message_id, std::unique_ptr<BaseStruct> &message)
        {
            pimpl_->broadcastMessage(message_id, message);
        }

        void NetService::processNetCommand(const NetCommand *cmd)
        {
            pimpl_->processNetCommand(cmd);
        }

    } // namespace common
} // namespace ele

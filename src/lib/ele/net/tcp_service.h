#ifndef ELE_NET_TCP_SERVICE_H
#define ELE_NET_TCP_SERVICE_H

#include <cstddef>
#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>
#include <stdint.h>

namespace ele
{
    class DynamicBuffer;

    namespace net
    {
        class IOService;
        class SocketAddr;
        class TcpSocket;

        class TcpService
        {
        public:
            class Context
            {
            public:
                Context() {}
                virtual ~Context() = 0;

            private:
                ELE_NONCOPYABLE(Context)
            };

            typedef int64_t SocketId;
            using NewConnectionCallback = std::function<void(TcpService *, SocketId, SocketId)>;
            using RecvMessageCallback = std::function<void(TcpService *, SocketId, DynamicBuffer *)>;
            using PeerCloseCallback = std::function<void(TcpService *, SocketId)>;
            using ErrorCallback = std::function<void(TcpService *, SocketId, int)>;
            using SendCompleteCallback = std::function<void(TcpService *, SocketId)>;

            explicit TcpService(IOService &io_service);
            ~TcpService();

            IOService *getIOService() const;

            SocketId listen(const SocketAddr &addr);
            SocketId shareListen(const TcpSocket &share_socket);
            SocketId connect(const SocketAddr &addr);
            SocketId asyncConnect(const SocketAddr &addr, bool *complete, int timeout_ms = -1);

            bool isConnected(SocketId socket_id) const;

            bool getLocalAddr(SocketId socket_id, SocketAddr *addr) const;
            bool getPeerAddr(SocketId socket_id, SocketAddr *addr) const;

            bool sendMessage(SocketId socket_id, const char *buffer, size_t size,
                             const SendCompleteCallback &send_complete_cb);
            bool sendMessageThenClose(SocketId socket_id, const char *buffer, size_t size);
        
            void broadcastMessage(const char *buffer, size_t size);
            void closeSocket(SocketId socket_id);

            Context *getContext(SocketId socket_id) const;
            bool setContext(SocketId socket_id, Context *context);

            void setNewConnectionCallback(const NewConnectionCallback &new_conn_cb);
            void setRecvMessageCallback(const RecvMessageCallback &recv_message_cb);
            void setPeerCloseCallback(const PeerCloseCallback &peer_close_cb);
            void setErrorCallback(const ErrorCallback &error_cb);

            void setRecvBufferInitSize(size_t size = 1024);
            void setRecvBufferExpandSize(size_t size = 1024);
            void setRecvBufferMaxSize(size_t size = 0);

            void setSendBufferInitSize(size_t size = 1024);
            void setSendBufferExpandSize(size_t size = 1024);
            void setSendBufferMaxSize(size_t size = 0);

            void setAcceptPauseTimeWhenExceedOpenFileLimit(int ms = 0);

        private:
            ELE_NONCOPYABLE(TcpService);

            class Impl;
            std::unique_ptr<Impl> pimpl_;
        }; // class TcpService
    }      // namespace net

} // namespace ele

#endif
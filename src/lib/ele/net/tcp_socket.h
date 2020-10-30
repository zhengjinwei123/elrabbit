#ifndef ELE_NET_TCP_SOCKET_H
#define ELE_NET_TCP_SOCKET_H
#include "io_device.h"
#include "socket_addr.h"

#include <cstddef>
#include <lib/ele/base/class_util.h>

namespace ele
{
    namespace net
    {
        class TcpSocket : public IODevice
        {
        public:
            TcpSocket();
            virtual ~TcpSocket();

            bool open(SocketAddr::Protocol::type protocol);
            void close();
            bool connect(const SocketAddr &addr);
            bool bind(const SocketAddr &addr);
            bool listen(int backlog);
            bool accept(TcpSocket *peer);

            bool getLocalAddr(SocketAddr *addr) const;
            bool getPeeAddr(SocketAddr *addr) const;

            int readableBytes() const;
            int recv(char *buffer, size_t size);
            int send(const char *buffer, size_t size);

            bool shutdownRead();
            bool shutdownWrite();
            bool shutdownReadAndWrite();

            int getSocketError();
            bool setReuseAddr();
            bool setTcpNoDelay();

            bool activeOpen(const SocketAddr &remote_addr);
            bool activeOpenNonblock(const SocketAddr &remote_addr);

            bool passiveOpen(const SocketAddr &local_addr);
            bool passiveOpenNonblock(const SocketAddr &local_addr);

            bool acceptNonblock(TcpSocket *peer);

        private:
            ELE_NONCOPYABLE(TcpSocket);
        };
    } // namespace net
} // namespace ele
#endif

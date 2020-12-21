#ifndef YUT_NET_TCP_SOCKET_H
#define YUT_NET_TCP_SOCKET_H

#include <cstddef>
#include <yut/base/class_util.h>
#include <yut/net/io_channel.h>
#include <yut/net/inet_address.h>

namespace yut {

namespace net {

class TcpSocket : public IOChannel {
public:
    TcpSocket();
    virtual ~TcpSocket();

    bool open(InetAddress::Protocol::type protocol);
    void close();

    bool connect(const InetAddress &addr);
    bool bind(const InetAddress &addr);
    bool listen(int backlog);
    bool accept(TcpSocket *peer);

    bool getLocalAddress(InetAddress *addr) const;
    bool getPeerAddress(InetAddress *addr) const;

    int readableBytes() const;
    int recv(char *buffer, size_t size);
    int send(const char *buffer, size_t size);

    bool shutdownRead();
    bool shutdownWrite();
    bool shutdownReadAndWrite();

    int getSocketError();
    bool setReuseAddr();
    bool setTcpNoDelay();

    // open setReuseAddr setTcpNoDelay connect
    bool activeOpen(const InetAddress &remote_addr);
    // activeOpen setNonblock
    bool activeOpenNonblock(const InetAddress &remote_addr);

    // open setReuseAddr setTcpNoDelay bind listen(128)
    bool passiveOpen(const InetAddress &local_addr);
    // accept setTcpNoDelay setNonblock
    bool passiveOpenNonblock(const InetAddress &local_addr);

    // accept setTcpNoDelay setNonblok
    bool acceptNonblock(TcpSocket *peer);

private:
    YUT_NONCOPYABLE(TcpSocket)
};

} // namespace net
} // namespace yut

#endif
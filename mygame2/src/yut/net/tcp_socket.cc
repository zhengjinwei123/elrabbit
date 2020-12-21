#include <yut/net/tcp_socket.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cerrno>

namespace yut {
namespace net {

TcpSocket::TcpSocket()
{

}

TcpSocket::~TcpSocket()
{
    close();
}

bool TcpSocket::open(InetAddress::Protocol::type protocol)
{
    if (fd_ != -1) {
        close();
    }

    if (protocol == InetAddress::Protocol::IP_V4) {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ == -1) {
            return false;
        }
    } else if (protocol == InetAddress::Protocol::IP_V6) {
        fd_ = ::socket(AF_INET6, SOCK_STREAM, 0);
        if (fd_ == -1) {
            return false;
        }
    } else {
        errno = EAFNOSUPPORT;
        return false;
    }

    if (setCloseOnExec() == false) {
        close();
        return false;
    }

    return true;
}

void TcpSocket::close()
{
    detachLoopService();
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool TcpSocket::connect(const InetAddress &addr)
{
    if (addr.getNativeAddress() == nullptr) {
        errno = EAFNOSUPPORT;
        return false;
    }

    if (::connect(fd_, (const struct sockaddr *)addr.getNativeAddress(), addr.getNativeAddressSize() != 0) {
        return false;
    }

    return true;
}

bool TcpSocket::bind(const InetAddress &addr)
{
    if (addr.getNativeAddress() == nullptr) {
        errno = EAFNOSUPPORT;
        return false;
    }

    if (::bind(fd_, (const struct sockaddr *)addr.getNativeAddress(), addr.getNativeAddressSize()) != 0) {
        return false;
    }
    return true;
}

bool TcpSocket::listen(int backlog)
{
    if (::listen(fd_, backlog) != 0) {
        return false;
    }
    return true;
}

bool TcpSocket::accept(TcpSocket *peer)
{
    int sock_fd = ::accept(fd_, nullptr, nullptr);
    if (-1 == sock_fd) {
        return false;
    }

    peer->setDescriptor(sock_fd);
    if (peer->setCloseOnExec() == false) {
        peer->close();
        return false;
    }
    return true;
}

bool TcpSocket::getLocalAddress(InetAddress *addr) const
{
    struct sockaddr_storage sock_addr;
    socklen_t addr_len = sizeof(sock_addr);
    if (::getsockname(fd_, (struct sockaddr *)&sock_addr, &addr_len) != 0) {
        return false;
    }
    return addr->setNativeAddress(&sock_addr);
}

bool TcpSocket::getPeerAddress(InetAddress *addr) const
{
    struct sockaddr_storage sock_addr;
    socklen_t addr_len = sizeof(sock_addr);
    if (::getpeername(fd_, (struct sockaddr *)&sock_addr, &addr_len) != 0) {
        return false;
    }
    return addr->setNativeAddress(&sock_addr);
}

int TcpSocket::readableBytes() const
{
    int readable_bytes = 0;
    if (::ioctl(fd_, FIONREAD, &readable_bytes) == -1) {
        return -1;
    }
    return readable_bytes;
}

int TcpSocket::recv(char *buffer, size_t size)
{
    return ::recv(fd_, buffer, size, 0);
}

int TcpSocket::send(const char *buffer, size_t size)
{
    return ::send(fd_, buffer, size, MSG_NOSIGNAL);
}

bool TcpSocket::shutdownRead()
{
    if (::shutdown(fd_, SHUT_RD) != 0) {
        return false;
    }

    return true;
}

bool TcpSocket::shutdownWrite()
{
    if (::shutdown(fd_, SHUT_WR) != 0) {
        return false;
    }

    return true;
}

bool TcpSocket::shutdownReadAndWrite()
{
    if (::shutdown(fd_, SHUT_RDWR) != 0) {
        return false;
    }

    return true;
}

int TcpSocket::getSocketError()
{
    int opt = 0;
    socklen_t opt_len = sizeof(opt);
    if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &opt, &opt_len) != 0) {
        return errno;
    }
    return opt;
}

bool TcpSocket::setReuseAddr()
{
    int opt = 1;
    if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
        return false;
    }

    return true;
}


bool TcpSocket::setTcpNoDelay()
{
    int opt = 1;
    if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) != 0) {
        return false;
    }

    return true;
}


// open setReuseAddr setTcpNoDelay connect
bool TcpSocket::activeOpen(const InetAddress &remote_addr)
{
    if (open(remote_addr.getProtocol()) == false) {
        return false;
    }

    if (setReuseAddr() == false ||
        setTcpNoDelay() == false ||
        connect(remote_addr) == false) {
        close();
        return false;
    }
    return true;
}

// activeOpen setNonblock
bool TcpSocket::activeOpenNonblock(const InetAddress &remote_addr)
{
    if (activeOpen(remote_addr) == false) {
        return false;
    }

    if (setNonblock() == false) {
        close();
        return false;
    }

    return true;
}

// open setReuseAddr setTcpNoDelay bind listen(128)
bool TcpSocket::passiveOpen(const InetAddress &local_addr)
{
    if (open(local_addr.getProtocol()) == false) {
        return false;
    }

    if (setReuseAddr() == false ||
        setTcpNoDelay() == false ||
        bind(local_addr) == false ||
        listen(128) == false) {
        close();
        return false;
    }

    return true;
}
// accept setTcpNoDelay setNonblock
bool TcpSocket::passiveOpenNonblock(const InetAddress &local_addr)
{
    if (passiveOpen(local_addr) == false) {
        return false;
    }

    if (setNonblock() == false) {
        close();
        return false;
    }

    return true;
}

// accept setTcpNoDelay setNonblok
bool TcpSocket::acceptNonblock(TcpSocket *peer)
{
    if (accept(peer) == false) {
        return false;
    }

    if (peer->setTcpNoDelay() == false ||
        peer->setNonblock() == false) {
        peer->close();
        return false;
    }

    return true;
}

} // namespace net
} // namespace yut
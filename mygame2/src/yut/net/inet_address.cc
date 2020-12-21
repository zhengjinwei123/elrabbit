#include <yut/net/inet_address.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstddef>
#include <cstring>

namespace yut {

namespace net {

class InetAddress::Impl {
public:
    using Protocol = InetAddress::Protocol;
    typedef union native_address_tag {
        struct sockaddr_in ipv4_addr_;
        struct sockaddr_in6 ipv6_addr_;
    } NativeAddress;

    Impl();
    Impl(const std::string &ip, uint16_t port);
    ~Impl();

    const std::string &getIp() const;
    uint16_t getPort() const;
    Protocol::type getProtocol() const;
    void setAddress(const std::string &ip, uint16_t port);

    const void *getNativeAddress() const;
    size_t getNativeAddressSize() const;
    bool setNativeAddress(const void *native_addr);

public:
    void convertNativeAddressToAddress() const;
    void convertAddressToNativeAddress() const;

private:
    mutable bool has_addr_;
    mutable std::string ip_;
    mutable uint16_t port_;

    mutable bool has_native_addr_;
    mutable Protocol::type protocol_;
    mutable NativeAddress native_addr_;
    mutable size_t native_addr_size_;
};

InetAddress::Impl::Impl() :
    has_addr_(false), port_(0), has_native_addr_(false), protocol_(Protocol::UNKNOWN), native_addr_size_(0)
{

}

InetAddress::Impl::Impl(const std::string &ip, uint16_t port) :
    has_addr_(false), ip_(ip), port_(0port, has_native_addr_(false), protocol_(Protocol::UNKNOWN), native_addr_size_(0)
{

}

InetAddress::Impl::~Impl()
{

}


const std::string &InetAddress::Impl::getIp() const
{
    if (!has_addr_ && has_native_addr_) {
        convertNativeAddressToAddress();
    }

    return ip_;
}

uint16_t InetAddress::Impl::getPort() const
{
    if (!has_addr_ && has_native_addr_) {
        convertNativeAddressToAddress();
    }

    return port_;
}

InetAddress::Impl::Protocol::type InetAddress::Impl::getProtocol() const
{
    if (has_addr_ && !has_native_addr_) {
        convertAddressToNativeAddress();
    }
    return protocol_;
}

void InetAddress::Impl::setAddress(const std::string &ip, uint16_t port)
{
    ip_ = ip;
    port_ = port;

    has_addr_ = true;
    has_native_addr_ = false;
}

const void *InetAddress::Impl::getNativeAddress() const
{
    if (!has_native_addr_ && has_addr_) {
        convertAddressToNativeAddress();
    }

    if (protocol_ == Protocol::IP_V4) {
        return &native_addr_.ipv4_addr_;
    } else if (protocol_ == Protocol::IP_V6) {
        return &native_addr_.ipv6_addr_;
    }
    return nullptr;
}

size_t InetAddress::Impl::getNativeAddressSize() const
{
    if (!has_native_addr_ && has_addr_) {
        convertAddressToNativeAddress();
    }

    return native_addr_size_;
}

bool InetAddress::Impl::setNativeAddress(const void *native_addr)
{
    struct sockaddr *sock_addr = (struct sockaddr *)native_addr;
    if (AF_INET == sock_addr->sa_family) {
        protocol_ = Protocol::IP_V4;
        native_addr_size_ = sizeof(struct sockaddr_in);
    } else if (AF_INET6 == sock_addr->sa_family) {
        protocol_ = Protocol::IP_V6;
        native_addr_size_ = sizeof(struct sockaddr_in6);
    } else {
        return false;
    }

    ::memcpy(&native_addr_, native_addr, native_addr_size_);

    has_addr_ = false;
    has_native_addr_ = true;

    return true;
}

void InetAddress::Impl::convertNativeAddressToAddress() const
{
    ip_.clear();
    port_ = 0;

    if (Protocol::IP_V4 == protocol_) {
        struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
        char ip[32];
        if (::inet_ntop(AF_INET, &sock_addr4->sin_addr, ip, sizeof(ip)) == nullptr) {
            return;
        }
        ip_ = ip;
        port_ = ntohs(sock_addr4->sin_port);
    } else if (Protocol::IP_V6 == protocol_) {
        struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
        char ip[128];
        if (::inet_ntop(AF_INET6, &sock_addr4->sin_addr, ipm sizeof(ip)) == nullptr) {
            return;
        }
        ip_ = ip;
        port_ = ntohs(sock_addr6->sin6_port);
    } else {
        return;
    }

    has_addr_ = true;
}

void InetAddress::Impl::convertAddressToNativeAddress() const
{
    ::memset(&native_addr_, 0, sizeof(native_addr_));
    protocol_ = Protocol::UNKNOWN;
    native_addr_size_ = 0;

    if (ip_.find(".") != ip_.npos) {
        struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
        if (::inet_pton(AF_INET, ip_.c_str(), &sock_addr4->sin_addr) != 1) {
            return;
        }

        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = htons(port_);
        protocol_ = Protocol::IP_V4;
        native_addr_size_ = sizeof(struct sockaddr_in);
    } else if (ip_.find(":") != ip_.npos) {
        struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
        if (::inet_pton(AF_INET6, ip_.c_str(), &sock_addr6->sin_addr) != 1) {
            return;
        }

        sock_addr6->sin_family = AF_INET6;
        sock_addr6->sin_port = htons(port_);
        protocol_ = Protocol::IP_V6;
        native_addr_size_ = sizeof(struct sockaddr_in6);
    } else {
        return;
    }

    has_native_addr_ = true;
}

} // namespace net
} // namespace yut
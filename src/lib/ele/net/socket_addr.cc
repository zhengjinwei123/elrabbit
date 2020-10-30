#include "socket_addr.h"

#include <arpa/inet.h>
#include <cstddef>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>

namespace ele
{
    namespace net
    {
        class SocketAddr::Impl
        {
        public:
            using Protocol = SocketAddr::Protocol;
            typedef union native_addr_tag
            {
                struct sockaddr_in ipv4_addr_;
                struct sockaddr_in6 ipv6_addr_;
            } NativeAddr;

            Impl();
            Impl(const std::string &ip, uint16_t port);
            ~Impl();

            const std::string &getIp() const;
            uint16_t getPort() const;
            Protocol::type getProtocol() const;
            void setAddr(const std::string &ip, uint16_t port);

            const void *getNativeAddr() const;
            size_t getNativeAddrSize() const;

            bool setNativeAddr(const void *native_addr);

        public:
            void translateNativeAddrToAddr() const;
            void translateAddrToNativeAddr() const;

        private:
            mutable bool has_addr_;
            mutable std::string ip_;
            mutable uint16_t port_;

            mutable bool has_native_addr_;
            mutable Protocol::type protocol_;
            mutable NativeAddr native_addr_;
            mutable size_t native_addr_size_;
        };

        SocketAddr::Impl::Impl() : has_addr_(false),
                                   port_(0),
                                   has_native_addr_(false),
                                   protocol_(Protocol::UNKNOWN),
                                   native_addr_size_(0)
        {
        }

        SocketAddr::Impl::Impl(const std::string &ip, uint16_t port) : has_addr_(true), ip_(ip),
                                                                       port_(port),
                                                                       has_native_addr_(false),
                                                                       protocol_(Protocol::UNKNOWN),
                                                                       native_addr_size_(0)
        {
        }

        SocketAddr::Impl::~Impl()
        {
        }

        const std::string &SocketAddr::Impl::getIp() const
        {
            if (!has_addr_ && has_native_addr_)
            {
                translateNativeAddrToAddr();
            }
            return ip_;
        }

        uint16_t SocketAddr::Impl::getPort() const
        {
            if (!has_addr_ && has_native_addr_)
            {
                translateNativeAddrToAddr();
            }
            return port_;
        }

        SocketAddr::Impl::Protocol::type SocketAddr::Impl::getProtocol() const
        {
            if (!has_native_addr_ && has_addr_)
            {
                translateAddrToNativeAddr();
            }
            return protocol_;
        }

        void SocketAddr::Impl::setAddr(const std::string &ip, uint16_t port)
        {
            ip_ = ip;
            port_ = port;

            has_addr_ = true;
            has_native_addr_ = false;
        }

        const void *SocketAddr::Impl::getNativeAddr() const
        {
            if (!has_native_addr_ && has_addr_)
            {
                translateAddrToNativeAddr();
            }

            if (Protocol::IPV4 == protocol_)
            {
                return &native_addr_.ipv4_addr_;
            }
            else if (Protocol::IPV6 == protocol_)
            {
                return &native_addr_.ipv6_addr_;
            }
            else
            {
                return nullptr;
            }
        }

        size_t SocketAddr::Impl::getNativeAddrSize() const
        {
            if (!has_native_addr_ && has_addr_)
            {
                translateAddrToNativeAddr();
            }

            return native_addr_size_;
        }

        bool SocketAddr::Impl::setNativeAddr(const void *native_addr)
        {
            struct sockaddr *sock_addr = (struct sockaddr *)native_addr;
            if (AF_INET == sock_addr->sa_family)
            {
                protocol_ = Protocol::IPV4;
                native_addr_size_ = sizeof(struct sockaddr_in);
            }
            else if (AF_INET6 == sock_addr->sa_family)
            {
                protocol_ = Protocol::IPV6;
                native_addr_size_ = sizeof(struct sockaddr_in6);
            }
            else
            {
                return false;
            }

            ::memcpy(&native_addr_, native_addr, native_addr_size_);

            has_addr_ = false;
            has_native_addr_ = true;

            return true;
        }

        void SocketAddr::Impl::translateNativeAddrToAddr() const
        {
            ip_.clear();
            port_ = 0;

            if (Protocol::IPV4 == protocol_)
            {
                struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
                char ip[32];
                if (::inet_ntop(AF_INET, &sock_addr4->sin_addr, ip, sizeof(ip)) == nullptr)
                {
                    return;
                }

                ip_ = ip;
                port_ = ntohs(sock_addr4->sin_port);
            }
            else if (Protocol::IPV6 == protocol_)
            {
                struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
                char ip[128];
                if (::inet_ntop(AF_INET6, &sock_addr6->sin6_addr, ip, sizeof(ip)) == nullptr)
                {
                    return;
                }

                ip_ = ip;
                port_ = ntohs(sock_addr6->sin6_port);
            }
            else
            {
                return;
            }

            has_addr_ = true;
        }

        void SocketAddr::Impl::translateAddrToNativeAddr() const
        {
            ::memset(&native_addr_, 0, sizeof(native_addr_));
            protocol_ = Protocol::UNKNOWN;
            native_addr_size_ = 0;

            if (ip_.find(".") != ip_.npos)
            {
                struct sockaddr_in *sock_addr4 = &native_addr_.ipv4_addr_;
                if (::inet_pton(AF_INET, ip_.c_str(), &sock_addr4->sin_addr) != 1)
                {
                    return;
                }
                sock_addr4->sin_family = AF_INET;
                sock_addr4->sin_port = htons(port_);
                protocol_ = Protocol::IPV4;
                native_addr_size_ = sizeof(struct sockaddr_in);
            }
            else if (ip_.find(":") != ip_.npos)
            {
                struct sockaddr_in6 *sock_addr6 = &native_addr_.ipv6_addr_;
                if (::inet_pton(AF_INET6, ip_.c_str(), &sock_addr6->sin6_addr) != 1)
                {
                    return;
                }
                sock_addr6->sin6_family = AF_INET6;
                sock_addr6->sin6_port = htons(port_);
                protocol_ = Protocol::IPV6;
                native_addr_size_ = sizeof(struct sockaddr_in6);
            }
            else
            {
                return;
            }

            has_native_addr_ = true;
        }

        SocketAddr::SocketAddr() : pimpl_(new Impl())
        {
        }

        SocketAddr::SocketAddr(const std::string &ip, uint16_t port) : pimpl_(new Impl(ip, port))
        {
        }

        SocketAddr::SocketAddr(const SocketAddr &copy) : pimpl_(new Impl(*copy.pimpl_))
        {
        }

        SocketAddr::~SocketAddr()
        {
        }

        SocketAddr &SocketAddr::operator=(const SocketAddr &rhs)
        {
            if (this == &rhs)
            {
                return *this;
            }

            *pimpl_ = *rhs.pimpl_;

            return *this;
        }

        const std::string &SocketAddr::getIp() const
        {
            return pimpl_->getIp();
        }

        uint16_t SocketAddr::getPort() const
        {
            return pimpl_->getPort();
        }

        SocketAddr::Protocol::type SocketAddr::getProtocol() const
        {
            return pimpl_->getProtocol();
        }

        void SocketAddr::setAddr(const std::string &ip, uint16_t port)
        {
            return pimpl_->setAddr(ip, port);
        }

        const void *SocketAddr::getNativeAddr() const
        {
            return pimpl_->getNativeAddr();
        }

        size_t SocketAddr::getNativeAddrSize() const
        {
            return pimpl_->getNativeAddrSize();
        }

        bool SocketAddr::setNativeAddr(const void *native_addr)
        {
            return pimpl_->setNativeAddr(native_addr);
        }

        bool SocketAddr::getAddrByDomain(const std::string &domain, std::vector<SocketAddr> *addr_list)
        {
            struct addrinfo *res = nullptr;
            struct addrinfo hints;

            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            int ret = ::getaddrinfo(domain.c_str(), nullptr, &hints, &res);
            if (ret != 0)
            {
                return false;
            }

            for (const struct addrinfo *p = res; p != nullptr; p = p->ai_next)
            {
                SocketAddr addr;
                addr.setNativeAddr(p->ai_addr);
                addr_list->push_back(addr);
            }

            ::freeaddrinfo(res);

            return true;
        }

        bool SocketAddr::ipV4TextToBin(const std::string &text, uint32_t *binary)
        {
            struct in_addr addr;
            ::memset(&addr, 0, sizeof(addr));
            if (::inet_pton(AF_INET, text.c_str(), &addr) != 1)
            {
                return false;
            }
            *binary = ntohl(addr.s_addr);

            return true;
        }

        void SocketAddr::ipV4BinToText(uint32_t binary, std::string *text)
        {
            struct in_addr addr;
            ::memset(&addr, 0, sizeof(addr));
            addr.s_addr = htonl(binary);

            char ip[32];
            if (::inet_ntop(AF_INET, &addr, ip, sizeof(ip)) == nullptr)
            {
                return;
            }

            *text = ip;
        }

        std::string SocketAddr::ipV4BinToText(uint32_t binary)
        {
            std::string text;
            ipV4BinToText(binary, &text);
            return text;
        }

        bool SocketAddr::ipV6TextToBin(const std::string &text, uint8_t binary[16])
        {
            struct in6_addr addr;
            ::memset(&addr, 0, sizeof(addr));
            if (::inet_pton(AF_INET6, text.c_str(), &addr) != 1)
            {
                return false;
            }

            for (int i = 0; i < 16; ++i)
            {
                binary[i] = addr.s6_addr[i];
            }

            return true;
        }

        void SocketAddr::ipV6BinToText(const uint8_t binary[16], std::string *text)
        {
            struct in6_addr addr;
            ::memset(&addr, 0, sizeof(addr));
            for (int i = 0; i < 16; ++i)
            {
                addr.s6_addr[i] = binary[i];
            }

            char ip[128];
            if (::inet_ntop(AF_INET6, &addr, ip, sizeof(ip)) == nullptr)
            {
                return;
            }
            *text = ip;
        }

        std::string SocketAddr::ipV6BinToText(const uint8_t binary[16])
        {
            std::string text;
            ipV6BinToText(binary, &text);
            return text;
        }

    } // namespace net
} // namespace ele
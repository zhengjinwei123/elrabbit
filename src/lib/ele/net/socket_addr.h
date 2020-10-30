#ifndef ELE_NET_SOCKET_ADDR_H
#define ELE_NET_SOCKET_ADDR_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

namespace ele
{
    namespace net
    {
        class SocketAddr
        {
        public:
            struct Protocol
            {
                enum type
                {
                    UNKNOWN = 0,
                    IPV4,
                    IPV6
                };
            };

            SocketAddr();
            SocketAddr(const std::string &ip, uint16_t port);
            SocketAddr(const SocketAddr &copy);
            ~SocketAddr();

            SocketAddr &operator=(const SocketAddr &rhs);

            const std::string &getIp() const;
            uint16_t getPort() const;
            Protocol::type getProtocol() const;
            void setAddr(const std::string &ip, uint16_t port);

            const void *getNativeAddr() const;
            size_t getNativeAddrSize() const;
            bool setNativeAddr(const void *native_addr);

            static bool getAddrByDomain(const std::string &domain, std::vector<SocketAddr> *addr_list);

            static bool ipV4TextToBin(const std::string &text, uint32_t *binary);
            static void ipV4BinToText(uint32_t binary, std::string *text);
            static std::string ipV4BinToText(uint32_t binary);

            static bool ipV6TextToBin(const std::string &text, uint8_t binary[16]);
            static void ipV6BinToText(const uint8_t binary[16], std::string *text);
            static std::string ipV6BinToText(const uint8_t binary[16]);

        private:
            class Impl;
            std::unique_ptr<Impl> pimpl_;
        }; // class SocketAddr
    }      // namespace net
} // namespace ele

#endif // ELE_NET_SOCKET_ADDR_H
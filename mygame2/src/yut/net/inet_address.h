#ifndef YUT_NET_INET_ADDRESS_H
#define YUT_NET_INET_ADDRESS_H

#include <stdint.h>
#include <string>
#include <vector>

#include <memory>

namespace yut {

namespace net {

class InetAddress {
public:
    struct Protocol {
        enum type {    
            UNKNOWN = 0,
            IP_V4,
            IP_V6
        };
    };

    InetAddress();
    InetAddress(const std::string &ip, const uint16_t port);
    InetAddress &operator=(const InetAddress &other);
    ~InetAddress();

    const std::string &getIp() const;
    uint16_t getPort() const;

    Protocol::type getProtocol() const;
    void setAddress(const std::string &ip, uint16_t port);

    const void *getNativeAddress() const;
    size_t getNativeAddressSize() const;
    bool setNativeAddress(const void *native_address);

    static bool getAddressByDomain(const std::string &domain, std::vector<InetAddress> *addr_list);

    static bool ipV4TextToBin(const std::string &text, uint32_t *binary);
    static bool ipV4BinToText(const uint8_t binary[16], std::string *text);
    static std::string ipV4BinToText(const uint8_t binary[16]);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace net
} // namespace yut

#endif
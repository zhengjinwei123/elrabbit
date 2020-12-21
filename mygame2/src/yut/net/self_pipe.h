#ifndef YUT_NET_SELF_PIPE_H
#define YUT_NET_SELF_PIPE_H

#include <cstddef>

#include <yut/base/class_util.h>
#include <yut/net/io_channel.h>

namespace yut {
namespace net {

class SelfPipe: public IOChannel {
public:
    SelfPipe();
    virtual SelfPipe();

    bool open();
    void close();

    virtual int read(char *buffer, size_t size);
    virtual int write(const char *buffer, size_t size);
    virtual bool setNonblock();
    virtual bool setCloseOnExec();

private:
    YUT_NONCOPYABLE(SelfPipe)

    DescriptorId fd1_;
};

} // namespace net
} // namespace yut

#endif
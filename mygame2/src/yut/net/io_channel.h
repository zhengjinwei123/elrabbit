#ifndef YUT_NET_IO_DEVICE_H
#define YUT_NET_IO_DEVICE_H

#include <stdint.h>
#include <cstddef>

#include <yut/base/class_util.h>
#include <functional>

namespace yut {
namespace net {

class LoopService;

class IOChannel {
public:
    typedef int DescriptorId;

    using ReadCallback = std::function<void (IOChannel *)>;
    using WriteCallback = std::function<void (IOChannel *)>;
    using ErrorCallback = std::function<void (IOChannel *)>;

    IOChannel();
    virtual ~IOChannel();

    bool attachLoopService(LoopService *loop_service);
    void detachLoopService();

    int64_t getId() const { return id_; }
    void setId(int64_t id) { id_ = id; }
    DescriptorId getDescriptor() const { return fd_; }
    void setDescriptor(DescriptorId fd) { fd_ = fd; }
    bool dupDescriptor(DescriptorId fd);

    const ReadCallback &getReadCallback() const { return read_cb_; }
    const WriteCallback &getWriteCallback() const { return write_cb_; }
    const ErrorCallback &getErrorCallback() const { return error_cb_; }

    void setReadCallback(const ReadCallback &read_cb);
    void setWriteCallback(const WriteCallback &write_cb);
    void setErrorCallback(const ErrorCallback &error_cb);

    virtual int read(char *buffer, size_t size);
    virtual int write(const char *buffer, size_t size);
    virtual bool setNonblock();
    virtual bool setCloseOnExec();

protected:
    LoopService *loop_service_;
    int64_t id_;
    DescriptorId fd_;
    ReadCallback read_cb_;
    WriteCallback write_cb_;
    ErrorCallback error_cb_;

private:
    YUT_NONCOPYABLE(LoopService)
};

} // namespace net
} // namespace yut

#endif
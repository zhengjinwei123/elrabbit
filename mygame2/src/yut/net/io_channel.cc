#include <yut/net/io_channel.h>

#include <fcntl.h>
#include <unistd.h>

#include <yut/net/loop_service.h>

namespace yut {

namespace net {

IOChannel::IOChannel() :
    loop_service_(nullptr), id_(0), fd_(-1)
{

}

IOChannel::~IOChannel()
{
    detachLoopService();
}

bool IOChannel::dupDescriptor(DescriptorId fd)
{
    int dup_fd = ::dup(fd);
    if (-1 == dup_fd) {
        return false;
    }

    fd_ = dup_fd;

    return true;
}

bool IOChannel::attachLoopService(LoopService &loop_service)
{
    if (loop_service_ != nullptr) {
        detachLoopService();
    }

    if (loop_service.addIOChannel(this) == false) {
        return false;
    }

    loop_service_ = &loop_service;

    return true;
}

void IOChannel::detachLoopService()
{
    if (nullptr == loop_service_) {
        return;
    }

    loop_service_->removeIOChannel(this);
    loop_service_ = nullptr;
}

void IOChannel::setReadCallback(const ReadCallback &read_cb)
{
    read_cb_ = read_cb;
    if (loop_service_ != nullptr) {
        loop_service_->updateIOChannel(this);
    }
}

void IOChannel::setWriteCallback(const WriteCallback &write_cb)
{
    write_cb_ = write_cb;
    if (loop_service_ != nullptr) {
        loop_service_->updateIOChannel(this);
    }
}
void IOChannel::setErrorCallback(const ErrorCallback &error_cb)
{
    error_cb_ = error_cb;
}

int IOChannel::write(const char *buffer, size_t size)
{
    return ::write(fd_, buffer, size);
}

int IOChannel::read(char *buffer, size_t size)
{
    return ::read(fd_, buffer, size);
}

bool IOChannel::setNonblock()
{
    int flags = ::fcntl(fd_, F_GETFL, 0);
    if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) != 0) {
        return false;
    }
    return true;
}

bool IOChannel::setCloseOnExec()
{
    int flags = ::fcntl(fd_, F_GETFD, 0);
    if (::fcntl(fd_, F_SETFD, flags | FD_CLOEXEC) != 0) {
        return false;
    }
    return true;
}



} // namespace net
} // namespace yut
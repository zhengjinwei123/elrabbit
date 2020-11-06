#ifndef ELE_NET_SELF_PIPE_H
#define ELE_NET_SELF_PIPE_H

#include "io_device.h"
#include <cstddef>
#include <lib/ele/base/class_util.h>

namespace ele
{
    namespace net
    {
        class SelfPipe : public IODevice
        {
        public:
            SelfPipe();
            virtual ~SelfPipe();

            bool open();
            void close();

            virtual int read(char *buffer, size_t size);
            virtual int write(const char *buffer, size_t size);
            virtual bool setNonblock();

            virtual bool setCloseOnExec();
        private:
            ELE_NONCOPYABLE(SelfPipe);

            DescriptorId fd1_;
        }; // SelfPipe

    }      // namespace net
} // namespace ele
#endif
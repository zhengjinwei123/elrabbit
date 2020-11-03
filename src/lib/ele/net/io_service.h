#ifndef ELE_NET_IO_SERVICE_H
#define ELE_NET_IO_SERVICE_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>
#include <stdint.h>

namespace ele
{
    namespace net
    {
        class IODevice;

        class IOService
        {
        public:
            typedef int64_t TimerId;
            using TimerCallback = std::function<void(TimerId)>;

            IOService();
            ~IOService();

            void loop();
            void quit();

            TimerId startTimer(int64_t timerout_ms, const TimerCallback &timer_cb, int call_times = -1);
            void stopTimer(TimerId timer_id);

        private:
            friend class IODevice;
            bool addIODevice(IODevice *io_device);
            bool removeIODevice(IODevice *io_device);
            bool updateIODevice(IODevice *io_device);

        private:
            ELE_NONCOPYABLE(IOService);

            class Impl;
            std::unique_ptr<Impl> pimpl_;
        };
    } // namespace net
} // namespace ele

#endif
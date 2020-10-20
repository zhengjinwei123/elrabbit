#ifndef ELE_BASE_SYSTEM_H
#define ELE_BASE_SYSTEM_H

namespace ele
{
    namespace system
    {
        bool daemon(bool change_dir = false, bool close_stdio = false);
        bool createPidFile(const char *file);

        typedef void (*SignalHandler)(int);
        SignalHandler signal(int signum, SignalHandler sighandler);
    } // namespace system

} // namespace ele

#endif
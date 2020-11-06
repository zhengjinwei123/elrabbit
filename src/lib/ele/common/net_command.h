#ifndef ELE_COMMON_NET_COMMAND_H
#define ELE_COMMON_NET_COMMAND_H

#include "net_id.h"
#include <lib/ele/base/class_util.h>

namespace ele
{
    namespace exchange
    {
        class BaseStruct;
    }

    namespace common
    {
        class NetCommand
        {
        public:
            struct Type
            {
                enum type
                {
                    NEW,
                    MESSAGE,
                    ENABLE_BROADCAST,
                    DISABLE_BROADCAST,
                    BROADCAST,
                    CLOSE,
                };
            };

            NetCommand(Type::type type_);
            ~NetCommand();

        public:
            Type::type type;
            NetId id;
            int message_id;
            ele::exchange::BaseStruct *message;

        private:
            ELE_NONCOPYABLE(NetCommand);
        };
    } // namespace common
} // namespace ele

#endif
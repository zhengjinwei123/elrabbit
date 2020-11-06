#include "net_command.h"

#include <cstddef>
#include <lib/ele/exchange/base_struct.h>

namespace ele
{
    namespace common
    {
        NetCommand::NetCommand(Type::type type_) : type(type_), message_id(0), message(nullptr)
        {
        }

        NetCommand::~NetCommand()
        {
            if (this->message != nullptr)
            {
                delete this->message;
            }
        }
    } // namespace common
} // namespace ele
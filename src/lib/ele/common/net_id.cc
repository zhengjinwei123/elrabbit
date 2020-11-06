#include "net_id.h"

namespace ele
{
    namespace common
    {
        size_t NetId::Hash::operator()(const NetId &net_id) const
        {
            return net_id.socket_id;
        }

        NetId::NetId() : service_id(-1), socket_id(-1)
        {
        }

        void NetId::reset()
        {
            this->service_id = -1;
            this->socket_id = -1;
        }

        bool NetId::operator==(const NetId &other) const
        {
            return this->service_id == other.service_id &&
                   this->socket_id == other.socket_id;
        }

        bool NetId::isInValid() const
        {
            return this->service_id == -1 ||
                   this->socket_id == -1;
        }
    } // namespace net
} // namespace ele
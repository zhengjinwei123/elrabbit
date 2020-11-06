#ifndef ELE_COMMON_NET_ID_H
#define ELE_COMMON_NET_ID_H

#include <cstddef>
#include <stdint.h>

namespace ele
{
    namespace common
    {
        class NetId
        {
            public:
                struct Hash
                {
                    size_t operator()(const NetId &net_id) const;
                };

                NetId();
                void reset();
                bool operator==(const NetId &other) const;

                bool isInValid() const;
            
            public:
                int service_id;
                int64_t socket_id;
        };
    } // namespace net
} // namespace ele
#endif
#ifndef ELE_COMMON_NET_PROTOCOL_H
#define ELE_COMMON_NET_PROTOCOL_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>

namespace ele
{
    class DynamicBuffer;

    namespace exchange
    {
        class BaseStruct;
    }

    namespace common
    {
        class NetProtocol
        {
        public:
            struct RetCode
            {
                enum type
                {
                    ERROR = -1,
                    WAITING_MORE_DATA = 0,
                    MESSAGE_READY = 1,
                };
            };

            using CreateMessageFunc = std::function<ele::exchange::BaseStruct *(int)>;

            NetProtocol(int max_packet_length, const CreateMessageFunc &create_message_func);
            ~NetProtocol();

            RetCode::type recvMessage(ele::DynamicBuffer *buffer, int *ret_message_id,
                                      std::unique_ptr<ele::exchange::BaseStruct> &ret_message);

            bool writeMessage(int message_id,
                              const ele::exchange::BaseStruct *message, ele::DynamicBuffer *buffer);

        private:
            ELE_NONCOPYABLE(NetProtocol);

            int max_packet_length_;
            CreateMessageFunc create_message_func_;
        };
    } // namespace common
} // namespace ele

#endif
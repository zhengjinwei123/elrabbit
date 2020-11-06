#include "net_protocol.h"

#include <lib/ele/base/class_util.h>
#include <lib/ele/exchange/base_struct.h>
#include <lib/ele/base/dynamic_buffer.h>

#include "log.h"

namespace ele
{
    namespace common
    {
        using ele::DynamicBuffer;
        using ele::exchange::BaseStruct;

        NetProtocol::NetProtocol(int max_packet_length,
                                 const CreateMessageFunc &create_message_func) : max_packet_length_(max_packet_length),
                                                                                 create_message_func_(create_message_func)
        {
        }

        NetProtocol::~NetProtocol()
        {
        }

        NetProtocol::RetCode::type NetProtocol::recvMessage(DynamicBuffer *buffer,
                                                            int *ret_message_id,
                                                            std::unique_ptr<BaseStruct> &ret_message)
        {
            // get message id
            uint16_t message_id = 0;
            if (buffer->peekInt16(message_id, 0) == false)
            {
                return RetCode::WAITING_MORE_DATA;
            }

            // get message length
            uint16_t message_length = 0;
            if (buffer->peekInt16(message_length, 2) == false)
            {
                return RetCode::WAITING_MORE_DATA;
            }

            // check message length
            if ((int)message_length + 4 > max_packet_length_)
            {
                LOG_ERROR("message_length(%d) is greater than %d", message_length, max_packet_length_);
                return RetCode::ERROR;
            }

            if (buffer->readableBytes() < (size_t)(message_length + 4))
            {
                return RetCode::WAITING_MORE_DATA;
            }

            // create message
            std::unique_ptr<BaseStruct> message(create_message_func_(message_id));
            if (message.get() == nullptr)
            {
                LOG_ERROR("message_id (%d) is invalid", message_id);
                return RetCode::ERROR;
            }

            if (message->decode(buffer->readBegin() + 4, message_length) < 0)
            {
                return RetCode::ERROR;
            }

            buffer->read(message_length + 4);
            *ret_message_id = message_id;
            ret_message.swap(message);

            return RetCode::MESSAGE_READY;
        }

        bool NetProtocol::writeMessage(int message_id, const BaseStruct *message, DynamicBuffer *buffer)
        {
            buffer->clear();

            // message id
            buffer->writeInt16(message_id);

            // write message
            int encode_size = message->encode(
                buffer->writeBegin() + 2,
                buffer->writableBytes() - 2);
            if (encode_size < 0)
            {
                return false;
            }

            // write msg length
            buffer->writeInt16(encode_size);
            buffer->write(encode_size);

            return true;
        }
    } // namespace common
} // namespace ele
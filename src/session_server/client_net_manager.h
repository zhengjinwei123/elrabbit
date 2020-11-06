#ifndef SESSION_SERVER_CLIENT_NET_MANAGER_H
#define SESSION_SERVER_CLIENT_NET_MANAGER_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <lib/ele/common/log.h>
#include <lib/ele/common/net_service.h>
#include <memory>

namespace ele
{
    namespace exchange
    {
        class BaseStruct;
    }

    namespace common
    {
        class NetId;
    }
} // namespace ele

namespace session_server
{
    class ClientNetManager : public ele::common::NetService
    {
    public:
        bool init();

        virtual void onConnect(const ele::common::NetId &net_id);
        virtual void onDisconnect(const ele::common::NetId &net_id);
        virtual void onExceedRequestFrequencyLimit(const ele::common::NetId &net_id);

        template <class T>
        void setMessageHandler(const std::function<void(const ele::common::NetId &, const T *)> &message_handler)
        {
            static MessageHandlerWrapper<T> wrapper;
            static const int message_id = 1;
            if (wrapper.func)
            {
                LOG_ERROR("(%d) handler is already set", message_id);
                return;
            }

            wrapper.func = message_handler;

            ele::common::NetService::setMessageHandler(message_id,
                                                       std::bind(&MessageHandlerWrapper<T>::call, &wrapper,
                                                                 std::placeholders::_1, std::placeholders::_2));
        }

        template <class T>
        void sendMessage(const ele::common::NetId &net_id,
                         std::unique_ptr<T> &message)
        {
            static const int message_id = 1;

            std::unique_ptr<ele::exchange::BaseStruct> wrap_message(message.release());
            ele::common::NetService::sendMessage(net_id, message_id, wrap_message);
        }

        template <class T>
        void broadcastMessage(std::unique_ptr<T> &message)
        {
            static const int message_id = 1;

            std::unique_ptr<ele::exchange::BaseStruct> wrap_message(message.release());
            ele::common::NetService::broadcastMessage(message_id, wrap_message);
        }

        template <class T>
        void sendError(const ele::common::NetId &net_id, int error_code)
        {
            static const int message_id = 1;
            sendError(net_id, message_id, error_code);
        }

    private:
        void sendError(const ele::common::NetId &net_id, int response_message_id, int error_code);

    private:
        ELE_SINGLETON(ClientNetManager);
    };
} // namespace session_server

#define sClientNetManager session_server::ClientNetManager::getInstance()

#endif
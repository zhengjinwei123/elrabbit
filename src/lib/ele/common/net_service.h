#ifndef ELE_COMMON_NET_SERVICE_H
#define ELE_COMMON_NET_SERVICE_H

#include <functional>
#include <lib/ele/base/class_util.h>
#include <memory>
#include <stdint.h>
#include <string>

namespace ele
{
    namespace exchange
    {
        class BaseStruct;
    }

    namespace common
    {
        class NetCommand;
        class NetId;

        class NetService
        {
        public:
            using NewNetCommandCallback = std::function<void(std::unique_ptr<NetCommand> &)>;
            using CreateMessageFunc = std::function<ele::exchange::BaseStruct *(int)>;

            using MessageHandler = std::function<void(const NetId &, const ele::exchange::BaseStruct *)>;

            NetService();
            virtual ~NetService();

            bool init(const std::string &service_name, int thread_num,
                      const std::string &ip, uint16_t port,
                      const NewNetCommandCallback &new_net_cmd_cb,
                      const CreateMessageFunc &create_message_func,
                      int max_recv_buffer_size = 0,
                      int max_recv_packet_length = 1024,
                      int max_send_buffer_size = 0,
                      int max_send_packet_length = 10240,
                      int max_request_per_second = 0);

            void start();
            void stop();

            MessageHandler getMessageHandler(int message_id) const;
            void setMessageHandler(int message_id,
                                   const MessageHandler &message_handler);

            void disconnect(const NetId &net_id);
            void sendMessage(const NetId &net_id, int message_id, std::unique_ptr<ele::exchange::BaseStruct> &message);

            void enableBroadcast(const NetId &net_id);
            void disableBroadcast(const NetId &net_id);
            void broadcastMessage(int message_id, std::unique_ptr<ele::exchange::BaseStruct> &message);

            void processNetCommand(const NetCommand *cmd);

            virtual void onConnect(const NetId &net_id) {}
            virtual void onDisconnect(const NetId &net_id) {}
            virtual void onExceedRequestFrequencyLimit(const NetId &net_id) {}

        protected:
            template <class T>
            struct MessageHandlerWrapper
            {
                void call(const ele::common::NetId &net_id,
                          const ele::exchange::BaseStruct *message)
                {
                    this->func(net_id, (T *)message);
                }

                std::function<void(const ele::common::NetId &, const T *)> func;
            };

        private:
            ELE_NONCOPYABLE(NetService);

            class Impl;
            std::unique_ptr<Impl> pimpl_;
        };
    } // namespace common
} // namespace ele
#endif
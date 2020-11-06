#include "session_server/client_net_manager.h"

#include "session_server/server_app.h"
#include <lib/ele/base/timestamp.h>
#include <memory>

namespace session_server
{
    using ele::Timestamp;
    using ele::common::NetId;

    ClientNetManager::ClientNetManager()
    {
    }

    ClientNetManager::~ClientNetManager()
    {
    }

    bool ClientNetManager::init()
    {
        if (ele::common::NetService::init("client_net",
                                          4, "0.0.0.0", 8008,
                                          std::bind(&ServerApp::pushClientNetCommand, sServerApp, std::placeholders::_1),
                                          nullptr,
                                          81920,
                                          40960,
                                          409600,
                                          40960,
                                          100) == false)
        {
            return false;
        }

        return true;
    }

    void ClientNetManager::onConnect(const NetId &net_id)
    {

    }

    void ClientNetManager::onDisconnect(const NetId &net_id)
    {

    }

    void ClientNetManager::onExceedRequestFrequencyLimit(const NetId &net_id)
    {

    }

    void ClientNetManager::sendError(const NetId &net_id, int response_message_id, int error_code)
    {

    }
} // namespace session_server

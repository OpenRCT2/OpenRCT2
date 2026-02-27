/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NetworkBase.h"
#include "NetworkPacket.h"
#include "NetworkConnection.h"

#ifndef DISABLE_NETWORK

namespace OpenRCT2::Network
{
    // Server lobby handlers - stub implementations for now
    void NetworkBase::ServerHandleLobbyJoin(Connection& connection, Packet& packet)
    {
        // TODO: Implement lobby join handling
    }

    void NetworkBase::ServerHandleLobbyLeave(Connection& connection, Packet& packet)
    {
        // TODO: Implement lobby leave handling
    }

    void NetworkBase::ServerHandleLobbyPlayerReady(Connection& connection, Packet& packet)
    {
        // TODO: Implement player ready handling
    }

    void NetworkBase::ServerHandleLobbyStartRequest(Connection& connection, Packet& packet)
    {
        // TODO: Implement start request handling
    }

    // Client lobby handlers - stub implementations for now
    void NetworkBase::Client_Handle_LOBBY_STATE_SYNC(Connection& connection, Packet& packet)
    {
        // TODO: Implement lobby state sync handling
    }

    void NetworkBase::Client_Handle_LOBBY_PLAYER_JOINED(Connection& connection, Packet& packet)
    {
        // TODO: Implement player joined handling
    }

    void NetworkBase::Client_Handle_LOBBY_PLAYER_LEFT(Connection& connection, Packet& packet)
    {
        // TODO: Implement player left handling
    }

    void NetworkBase::Client_Handle_LOBBY_PLAYER_READY(Connection& connection, Packet& packet)
    {
        // TODO: Implement player ready handling
    }

} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK

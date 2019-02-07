/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_NETWORK
#    include "../common.h"
#    include "NetworkKey.h"
#    include "NetworkPacket.h"
#    include "NetworkTypes.h"
#    include "TcpSocket.h"

#    include <list>
#    include <memory>
#    include <vector>

class NetworkPlayer;
struct ObjectRepositoryItem;

class NetworkConnection final
{
public:
    std::unique_ptr<ITcpSocket> Socket = nullptr;
    NetworkPacket InboundPacket;
    NETWORK_AUTH AuthStatus = NETWORK_AUTH_NONE;
    NetworkStats_t Stats = {};
    NetworkPlayer* Player = nullptr;
    uint32_t PingTime = 0;
    NetworkKey Key;
    std::vector<uint8_t> Challenge;
    std::vector<const ObjectRepositoryItem*> RequestedObjects;

    NetworkConnection();
    ~NetworkConnection();

    int32_t ReadPacket();
    void QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front = false);
    void SendQueuedPackets();
    void ResetLastPacketTime();
    bool ReceivedPacketRecently();

    const utf8* GetLastDisconnectReason() const;
    void SetLastDisconnectReason(const utf8* src);
    void SetLastDisconnectReason(const rct_string_id string_id, void* args = nullptr);

private:
    std::list<std::unique_ptr<NetworkPacket>> _outboundPackets;
    uint32_t _lastPacketTime = 0;
    utf8* _lastDisconnectReason = nullptr;

    void RecordPacketStats(const NetworkPacket& packet, bool sending);
    bool SendPacket(NetworkPacket& packet);
};

#endif // DISABLE_NETWORK

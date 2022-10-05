/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#    include "Socket.h"

#    include <deque>
#    include <memory>
#    include <string_view>
#    include <vector>

class NetworkPlayer;
struct ObjectRepositoryItem;

class NetworkConnection final
{
public:
    std::unique_ptr<ITcpSocket> Socket = nullptr;
    NetworkPacket InboundPacket;
    NetworkAuth AuthStatus = NetworkAuth::None;
    NetworkStats_t Stats = {};
    NetworkPlayer* Player = nullptr;
    uint32_t PingTime = 0;
    NetworkKey Key;
    std::vector<uint8_t> Challenge;
    std::vector<const ObjectRepositoryItem*> RequestedObjects;
    bool ShouldDisconnect = false;

    NetworkConnection() noexcept;

    NetworkReadPacket ReadPacket();
    void QueuePacket(NetworkPacket&& packet, bool front = false);
    void QueuePacket(const NetworkPacket& packet, bool front = false)
    {
        auto copy = packet;
        return QueuePacket(std::move(copy), front);
    }

    // This will not immediately disconnect the client. The disconnect
    // will happen post-tick.
    void Disconnect() noexcept;

    bool IsValid() const;
    void SendQueuedPackets();
    void ResetLastPacketTime() noexcept;
    bool ReceivedPacketRecently() const noexcept;

    const utf8* GetLastDisconnectReason() const noexcept;
    void SetLastDisconnectReason(std::string_view src);
    void SetLastDisconnectReason(const StringId string_id, void* args = nullptr);

private:
    std::deque<NetworkPacket> _outboundPackets;
    uint32_t _lastPacketTime = 0;
    std::string _lastDisconnectReason;

    void RecordPacketStats(const NetworkPacket& packet, bool sending);
    bool SendPacket(NetworkPacket& packet);
};

#endif // DISABLE_NETWORK

/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_NETWORK

    #include "NetworkKey.h"
    #include "NetworkPacket.h"
    #include "NetworkTypes.h"
    #include "Socket.h"

    #include <memory>
    #include <string_view>
    #include <vector>

namespace OpenRCT2
{
    struct ObjectRepositoryItem;
}

namespace OpenRCT2::Network
{
    class Player;

    class Connection final
    {
    public:
        std::unique_ptr<ITcpSocket> Socket = nullptr;
        Packet InboundPacket;
        Auth AuthStatus = Auth::none;
        Stats stats = {};
        Player* player = nullptr;
        uint32_t PingTime = 0;
        Key key;
        std::vector<uint8_t> Challenge;
        std::vector<const ObjectRepositoryItem*> RequestedObjects;
        bool ShouldDisconnect = false;

        Connection() noexcept;

        void update();
        ReadPacket readPacket();
        void QueuePacket(const Packet& packet, bool front = false);

        Command getPendingPacketCommand() const noexcept;
        size_t getPendingPacketSize() const noexcept;
        size_t getPendingPacketAvailable() const noexcept;

        // This will not immediately disconnect the client. The disconnect
        // will happen post-tick.
        void Disconnect() noexcept;

        bool IsValid() const;
        void SendQueuedData();
        bool ReceivedDataRecently() const noexcept;

        const utf8* GetLastDisconnectReason() const noexcept;
        void SetLastDisconnectReason(std::string_view src);
        void SetLastDisconnectReason(StringId string_id, void* args = nullptr);

    private:
        std::vector<uint8_t> _inboundBuffer;
        std::vector<uint8_t> _outboundBuffer;
        uint32_t _lastReceiveTime = 0;
        std::string _lastDisconnectReason;
        bool _isLegacyProtocol = false;

        void RecordPacketStats(const Packet& packet, bool sending);
        void receiveData();
    };
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK

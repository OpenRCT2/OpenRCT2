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
        std::unique_ptr<ITcpSocket> socket = nullptr;
        Packet inboundPacket;
        Auth authStatus = Auth::none;
        Stats stats = {};
        Player* player = nullptr;
        uint32_t pingTime = 0;
        Key key;
        std::vector<uint8_t> challenge;
        std::vector<const ObjectRepositoryItem*> requestedObjects;
        bool shouldDisconnect = false;

        Connection() noexcept;

        void update();
        ReadPacket readPacket();
        void queuePacket(const Packet& packet, bool front = false);

        Command getPendingPacketCommand() const noexcept;
        size_t getPendingPacketSize() const noexcept;
        size_t getPendingPacketAvailable() const noexcept;

        // This will not immediately disconnect the client. The disconnect
        // will happen post-tick.
        void disconnect() noexcept;

        bool isValid() const;
        void sendQueuedData();
        bool receivedDataRecently() const noexcept;

        const utf8* getLastDisconnectReason() const noexcept;
        void setLastDisconnectReason(std::string_view src);
        void setLastDisconnectReason(StringId string_id, void* args = nullptr);

    private:
        std::vector<uint8_t> _inboundBuffer;
        std::vector<uint8_t> _outboundBuffer;
        uint32_t _lastReceiveTime = 0;
        std::string _lastDisconnectReason;
        bool _isLegacyProtocol = false;

        void recordPacketStats(const Packet& packet, bool sending);
        void receiveData();
    };
} // namespace OpenRCT2::Network

#endif // DISABLE_NETWORK

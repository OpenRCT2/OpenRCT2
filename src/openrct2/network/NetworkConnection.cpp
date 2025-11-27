/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkConnection.h"

    #include "../Diagnostic.h"
    #include "../core/Diagnostics.hpp"
    #include "../core/String.hpp"
    #include "../localisation/Formatting.h"
    #include "../platform/Platform.h"
    #include "Network.h"
    #include "Socket.h"

    #include <sfl/small_vector.hpp>

namespace OpenRCT2::Network
{
    static constexpr size_t kDisconnectReasonBufSize = 256;
    static constexpr size_t kBufferSize = 1024 * 16; // 16 KiB.
    #ifndef DEBUG
    static constexpr size_t kNoDataTimeout = 20; // Seconds.
    #endif

    static_assert(kBufferSize <= std::numeric_limits<uint16_t>::max(), "kBufferSize too big, uint16_t is max.");

    Connection::Connection() noexcept
    {
        ResetLastPacketTime();
    }

    void Connection::update()
    {
        if (!IsValid())
        {
            return;
        }

        receiveData();
        SendQueuedData();
    }

    void Connection::receiveData()
    {
        uint8_t buffer[kBufferSize];
        size_t bytesRead = 0;

        ReadPacket status = Socket->ReceiveData(buffer, sizeof(buffer), &bytesRead);
        if (status == ReadPacket::disconnected)
        {
            Disconnect();
            return;
        }

        if (status == ReadPacket::success)
        {
            _inboundBuffer.insert(_inboundBuffer.end(), buffer, buffer + bytesRead);
        }
    }

    ReadPacket Connection::readPacket()
    {
        if (_inboundBuffer.size() < sizeof(InboundPacket.Header))
        {
            return ReadPacket::moreData;
        }

        // Read packet header.
        auto& header = InboundPacket.Header;
        std::memcpy(&header, _inboundBuffer.data(), sizeof(header));

        // Normalise values.
        header.Size = Convert::NetworkToHost(header.Size);
        header.Id = ByteSwapBE(header.Id);

        // NOTE: For compatibility reasons for the master server we need to remove sizeof(Header.Id) from the size.
        // Previously the Id field was not part of the header rather part of the body.
        // We correct the size to have only the length of the body.
        if (header.Size < sizeof(header.Id))
        {
            // This is a malformed packet, disconnect.
            LOG_INFO(
                "Received malformed packet (size: %u) from {%s}, disconnecting.", header.Size, Socket->GetIpAddress().c_str());

            Disconnect();
            return ReadPacket::disconnected;
        }

        header.Size -= sizeof(header.Id);

        const auto totalPacketLength = sizeof(header) + header.Size;
        if (_inboundBuffer.size() < totalPacketLength)
        {
            return ReadPacket::moreData;
        }

        // Read packet body.
        InboundPacket.BytesTransferred = totalPacketLength;
        InboundPacket.Write(_inboundBuffer.data() + sizeof(header), header.Size);

        // Remove read data from buffer.
        _inboundBuffer.erase(_inboundBuffer.begin(), _inboundBuffer.begin() + totalPacketLength);

        RecordPacketStats(InboundPacket, false);

        return ReadPacket::success;
    }

    static sfl::small_vector<uint8_t, 512> serializePacket(const Packet& packet)
    {
        // NOTE: For compatibility reasons for the master server we need to add sizeof(Header.Id) to the size.
        // Previously the Id field was not part of the header rather part of the body.
        const auto bodyLength = packet.Data.size() + sizeof(packet.Header.Id);

        Guard::Assert(bodyLength <= std::numeric_limits<uint16_t>::max(), "Packet size too large");

        auto header = packet.Header;
        header.Size = static_cast<uint16_t>(bodyLength);
        header.Size = Convert::HostToNetwork(header.Size);
        header.Id = ByteSwapBE(header.Id);

        sfl::small_vector<uint8_t, 512> buffer;
        buffer.reserve(sizeof(header) + packet.Data.size());

        buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(header));
        buffer.insert(buffer.end(), packet.Data.begin(), packet.Data.end());

        return buffer;
    }

    void Connection::QueuePacket(const Packet& packet, bool front)
    {
        if (AuthStatus == Auth::ok || !packet.CommandRequiresAuth())
        {
            const auto payload = serializePacket(packet);
            if (front)
            {
                _outboundBuffer.insert(_outboundBuffer.begin(), payload.begin(), payload.end());
            }
            else
            {
                _outboundBuffer.insert(_outboundBuffer.end(), payload.begin(), payload.end());
            }

            RecordPacketStats(packet, true);
        }
    }

    void Connection::Disconnect() noexcept
    {
        ShouldDisconnect = true;
    }

    bool Connection::IsValid() const
    {
        return !ShouldDisconnect && Socket->GetStatus() == SocketStatus::connected;
    }

    void Connection::SendQueuedData()
    {
        if (_outboundBuffer.empty())
        {
            return;
        }

        const auto bytesSent = Socket->SendData(_outboundBuffer.data(), _outboundBuffer.size());

        if (bytesSent > 0)
        {
            _outboundBuffer.erase(_outboundBuffer.begin(), _outboundBuffer.begin() + bytesSent);
        }
    }

    void Connection::ResetLastPacketTime() noexcept
    {
        _lastPacketTime = Platform::GetTicks();
    }

    bool Connection::ReceivedPacketRecently() const noexcept
    {
    #ifndef DEBUG
        constexpr auto kTimeoutMs = kNoDataTimeout * 1000;
        if (Platform::GetTicks() > _lastPacketTime + kTimeoutMs)
        {
            return false;
        }
    #endif
        return true;
    }

    const utf8* Connection::GetLastDisconnectReason() const noexcept
    {
        return this->_lastDisconnectReason.c_str();
    }

    void Connection::SetLastDisconnectReason(std::string_view src)
    {
        _lastDisconnectReason = src;
    }

    void Connection::SetLastDisconnectReason(const StringId string_id, void* args)
    {
        char buffer[kDisconnectReasonBufSize];
        FormatStringLegacy(buffer, kDisconnectReasonBufSize, string_id, args);
        SetLastDisconnectReason(buffer);
    }

    void Connection::RecordPacketStats(const Packet& packet, bool sending)
    {
        uint32_t packetSize = static_cast<uint32_t>(packet.BytesTransferred);
        StatisticsGroup trafficGroup;

        switch (packet.GetCommand())
        {
            case Command::gameAction:
                trafficGroup = StatisticsGroup::Commands;
                break;
            case Command::map:
                trafficGroup = StatisticsGroup::MapData;
                break;
            default:
                trafficGroup = StatisticsGroup::Base;
                break;
        }

        if (sending)
        {
            stats.bytesSent[EnumValue(trafficGroup)] += packetSize;
            stats.bytesSent[EnumValue(StatisticsGroup::Total)] += packetSize;
        }
        else
        {
            stats.bytesReceived[EnumValue(trafficGroup)] += packetSize;
            stats.bytesReceived[EnumValue(StatisticsGroup::Total)] += packetSize;
        }
    }

} // namespace OpenRCT2::Network

#endif

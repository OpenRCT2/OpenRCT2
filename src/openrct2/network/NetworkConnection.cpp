/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    #include "../core/Guard.hpp"
    #include "../core/String.hpp"
    #include "../localisation/Formatting.h"
    #include "../platform/Platform.h"
    #include "Network.h"
    #include "Socket.h"

    #include <sfl/small_vector.hpp>

namespace OpenRCT2::Network
{
    static constexpr size_t kDisconnectReasonBufSize = 256;
    static constexpr size_t kBufferSize = 1024 * 128; // 128 KiB.
    static constexpr size_t kNoDataTimeout = 40;      // Seconds.

    Connection::Connection() noexcept
    {
        _lastReceiveTime = Platform::GetTicks();
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
            _lastReceiveTime = Platform::GetTicks();
            _inboundBuffer.insert(_inboundBuffer.end(), buffer, buffer + bytesRead);
        }
    }

    ReadPacket Connection::readPacket()
    {
        uint32_t magic = 0;

        // Check if we have enough data for the magic.
        if (_inboundBuffer.size() < sizeof(magic))
        {
            return ReadPacket::moreData;
        }

        // Read magic.
        std::memcpy(&magic, _inboundBuffer.data(), sizeof(magic));

        size_t totalPacketLength = 0;
        size_t headerSize = 0;

        magic = Convert::NetworkToHost(magic);
        if (magic == PacketHeader::kMagic)
        {
            // New format.
            auto& header = InboundPacket.Header;
            std::memcpy(&header, _inboundBuffer.data(), sizeof(header));

            header.magic = magic;
            header.version = Convert::NetworkToHost(header.version);
            header.size = Convert::NetworkToHost(header.size);
            header.id = Convert::NetworkToHost(header.id);

            headerSize = sizeof(header);
            totalPacketLength = sizeof(header) + header.size;
        }
        else
        {
            // Legacy format.
            PacketLegacyHeader header{};
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
                    "Received malformed packet (size: %u) from {%s}, disconnecting.", header.Size,
                    Socket->GetIpAddress().c_str());

                Disconnect();
                return ReadPacket::disconnected;
            }

            header.Size -= sizeof(header.Id);

            // Fill in new header format.
            InboundPacket.Header.magic = PacketHeader::kMagic;
            InboundPacket.Header.size = header.Size;
            InboundPacket.Header.id = header.Id;

            headerSize = sizeof(header);
            totalPacketLength = sizeof(header) + header.Size;

            _isLegacyProtocol = true;
        }

        if (_inboundBuffer.size() < totalPacketLength)
        {
            InboundPacket.BytesTransferred = _inboundBuffer.size();
            return ReadPacket::moreData;
        }

        // Read packet body.
        InboundPacket.BytesTransferred = totalPacketLength;
        InboundPacket.Write(_inboundBuffer.data() + headerSize, totalPacketLength - headerSize);

        // Remove read data from buffer.
        _inboundBuffer.erase(_inboundBuffer.begin(), _inboundBuffer.begin() + totalPacketLength);

        RecordPacketStats(InboundPacket, false);

        return ReadPacket::success;
    }

    static sfl::small_vector<uint8_t, 512> serializePacket(bool legacyProtocol, const Packet& packet)
    {
        sfl::small_vector<uint8_t, 512> buffer;

        if (legacyProtocol)
        {
            // NOTE: For compatibility reasons for the master server we need to add sizeof(Header.Id) to the size.
            // Previously the Id field was not part of the header rather part of the body.
            const auto bodyLength = packet.Data.size() + sizeof(PacketLegacyHeader::Id);

            Guard::Assert(bodyLength <= std::numeric_limits<uint16_t>::max(), "Packet size too large");

            PacketLegacyHeader header{};
            header.Size = static_cast<uint16_t>(bodyLength);
            header.Size = Convert::HostToNetwork(header.Size);
            header.Id = ByteSwapBE(packet.Header.id);

            buffer.insert(
                buffer.end(), reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(header));
        }
        else
        {
            PacketHeader header{};
            header.magic = Convert::HostToNetwork(PacketHeader::kMagic);
            header.version = Convert::HostToNetwork(PacketHeader::kVersion);
            header.size = Convert::HostToNetwork(static_cast<uint32_t>(packet.Data.size()));
            header.id = Convert::HostToNetwork(packet.Header.id);

            buffer.insert(
                buffer.end(), reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(header));
        }

        buffer.insert(buffer.end(), packet.Data.begin(), packet.Data.end());

        return buffer;
    }

    void Connection::QueuePacket(const Packet& packet, bool front)
    {
        if (AuthStatus == Auth::ok || !packet.CommandRequiresAuth())
        {
            const auto payload = serializePacket(_isLegacyProtocol, packet);
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

    bool Connection::ReceivedDataRecently() const noexcept
    {
        constexpr auto kTimeoutMs = kNoDataTimeout * 1000;

        const auto timeSinceLastRecv = Platform::GetTicks() - _lastReceiveTime;
        if (timeSinceLastRecv > kTimeoutMs)
        {
            return false;
        }

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

    Command Connection::getPendingPacketCommand() const noexcept
    {
        return InboundPacket.GetCommand();
    }

    size_t Connection::getPendingPacketSize() const noexcept
    {
        return InboundPacket.Header.size;
    }

    size_t Connection::getPendingPacketAvailable() const noexcept
    {
        return InboundPacket.BytesTransferred;
    }

} // namespace OpenRCT2::Network

#endif

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
        if (!isValid())
        {
            return;
        }

        receiveData();
        sendQueuedData();
    }

    void Connection::receiveData()
    {
        uint8_t buffer[kBufferSize];
        size_t bytesRead = 0;

        ReadPacket status = socket->ReceiveData(buffer, sizeof(buffer), &bytesRead);
        if (status == ReadPacket::disconnected)
        {
            disconnect();
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
            auto& header = inboundPacket.Header;
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
            header.size = Convert::NetworkToHost(header.size);
            header.id = ByteSwapBE(header.id);

            // NOTE: For compatibility reasons for the master server we need to remove sizeof(Header.id) from the size.
            // Previously the id field was not part of the header rather part of the body.
            // We correct the size to have only the length of the body.
            if (header.size < sizeof(header.id))
            {
                // This is a malformed packet, disconnect.
                LOG_INFO(
                    "Received malformed packet (size: %u) from {%s}, disconnecting.", header.size,
                    socket->GetIpAddress().c_str());

                disconnect();
                return ReadPacket::disconnected;
            }

            header.size -= sizeof(header.id);

            // Fill in new header format.
            inboundPacket.Header.magic = PacketHeader::kMagic;
            inboundPacket.Header.size = header.size;
            inboundPacket.Header.id = header.id;

            headerSize = sizeof(header);
            totalPacketLength = sizeof(header) + header.size;

            _isLegacyProtocol = true;
        }

        if (_inboundBuffer.size() < totalPacketLength)
        {
            inboundPacket.BytesTransferred = _inboundBuffer.size();
            return ReadPacket::moreData;
        }

        // Read packet body.
        inboundPacket.BytesTransferred = totalPacketLength;
        inboundPacket.Write(_inboundBuffer.data() + headerSize, totalPacketLength - headerSize);

        // Remove read data from buffer.
        _inboundBuffer.erase(_inboundBuffer.begin(), _inboundBuffer.begin() + totalPacketLength);

        recordPacketStats(inboundPacket, false);

        return ReadPacket::success;
    }

    static sfl::small_vector<uint8_t, 512> serializePacket(bool legacyProtocol, const Packet& packet)
    {
        sfl::small_vector<uint8_t, 512> buffer;

        if (legacyProtocol)
        {
            // NOTE: For compatibility reasons for the master server we need to add sizeof(Header.id) to the size.
            // Previously the id field was not part of the header rather part of the body.
            const auto bodyLength = packet.Data.size() + sizeof(PacketLegacyHeader::id);

            Guard::Assert(bodyLength <= std::numeric_limits<uint16_t>::max(), "Packet size too large");

            PacketLegacyHeader header{};
            header.size = static_cast<uint16_t>(bodyLength);
            header.size = Convert::HostToNetwork(header.size);
            header.id = ByteSwapBE(packet.Header.id);

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

    void Connection::queuePacket(const Packet& packet, bool front)
    {
        if (authStatus == Auth::ok || !packet.CommandRequiresAuth())
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

            recordPacketStats(packet, true);
        }
    }

    void Connection::disconnect() noexcept
    {
        shouldDisconnect = true;
    }

    bool Connection::isValid() const
    {
        return !shouldDisconnect && socket->GetStatus() == SocketStatus::connected;
    }

    void Connection::sendQueuedData()
    {
        if (_outboundBuffer.empty())
        {
            return;
        }

        const auto bytesSent = socket->SendData(_outboundBuffer.data(), _outboundBuffer.size());

        if (bytesSent > 0)
        {
            _outboundBuffer.erase(_outboundBuffer.begin(), _outboundBuffer.begin() + bytesSent);
        }
    }

    bool Connection::receivedDataRecently() const noexcept
    {
        constexpr auto kTimeoutMs = kNoDataTimeout * 1000;

        const auto timeSinceLastRecv = Platform::GetTicks() - _lastReceiveTime;
        if (timeSinceLastRecv > kTimeoutMs)
        {
            return false;
        }

        return true;
    }

    const utf8* Connection::getLastDisconnectReason() const noexcept
    {
        return this->_lastDisconnectReason.c_str();
    }

    void Connection::setLastDisconnectReason(std::string_view src)
    {
        _lastDisconnectReason = src;
    }

    void Connection::setLastDisconnectReason(const StringId string_id, void* args)
    {
        char buffer[kDisconnectReasonBufSize];
        FormatStringLegacy(buffer, kDisconnectReasonBufSize, string_id, args);
        setLastDisconnectReason(buffer);
    }

    void Connection::recordPacketStats(const Packet& packet, bool sending)
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
        return inboundPacket.GetCommand();
    }

    size_t Connection::getPendingPacketSize() const noexcept
    {
        return inboundPacket.Header.size;
    }

    size_t Connection::getPendingPacketAvailable() const noexcept
    {
        return inboundPacket.BytesTransferred;
    }

} // namespace OpenRCT2::Network

#endif

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

    #include "../core/String.hpp"
    #include "../localisation/Formatting.h"
    #include "../platform/Platform.h"
    #include "Socket.h"
    #include "network.h"

    #include <sfl/small_vector.hpp>

using namespace OpenRCT2;

static constexpr size_t kNetworkDisconnectReasonBufSize = 256;
static constexpr size_t kNetworkBufferSize = (1024 * 64) - 1; // 64 KiB, maximum packet size.
    #ifndef DEBUG
static constexpr size_t kNetworkNoDataTimeout = 20; // Seconds.
    #endif

static_assert(kNetworkBufferSize <= std::numeric_limits<uint16_t>::max(), "kNetworkBufferSize too big, uint16_t is max.");

NetworkConnection::NetworkConnection() noexcept
{
    ResetLastPacketTime();
}

NetworkReadPacket NetworkConnection::ReadPacket()
{
    size_t bytesRead = 0;

    // Read packet header.
    auto& header = InboundPacket.Header;
    if (InboundPacket.BytesTransferred < sizeof(InboundPacket.Header))
    {
        const size_t missingLength = sizeof(header) - InboundPacket.BytesTransferred;

        uint8_t* buffer = reinterpret_cast<uint8_t*>(&InboundPacket.Header);

        NetworkReadPacket status = Socket->ReceiveData(buffer, missingLength, &bytesRead);
        if (status != NetworkReadPacket::Success)
        {
            return status;
        }

        InboundPacket.BytesTransferred += bytesRead;
        if (InboundPacket.BytesTransferred < sizeof(InboundPacket.Header))
        {
            // If still not enough data for header, keep waiting.
            return NetworkReadPacket::MoreData;
        }

        // Normalise values.
        header.Size = Convert::NetworkToHost(header.Size);
        header.Id = ByteSwapBE(header.Id);

        // NOTE: For compatibility reasons for the master server we need to remove sizeof(Header.Id) from the size.
        // Previously the Id field was not part of the header rather part of the body.
        header.Size -= std::min<uint16_t>(header.Size, sizeof(header.Id));

        // Fall-through: Read rest of packet.
    }

    // Read packet body.
    {
        // NOTE: BytesTransfered includes the header length, this will not underflow.
        const size_t missingLength = header.Size - (InboundPacket.BytesTransferred - sizeof(header));

        uint8_t buffer[kNetworkBufferSize];

        if (missingLength > 0)
        {
            NetworkReadPacket status = Socket->ReceiveData(buffer, std::min(missingLength, kNetworkBufferSize), &bytesRead);
            if (status != NetworkReadPacket::Success)
            {
                return status;
            }

            InboundPacket.BytesTransferred += bytesRead;
            InboundPacket.Write(buffer, bytesRead);
        }

        if (InboundPacket.Data.size() == header.Size)
        {
            // Received complete packet.
            _lastPacketTime = Platform::GetTicks();

            RecordPacketStats(InboundPacket, false);

            return NetworkReadPacket::Success;
        }
    }

    return NetworkReadPacket::MoreData;
}

static sfl::small_vector<uint8_t, 512> serializePacket(const NetworkPacket& packet)
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

void NetworkConnection::QueuePacket(const NetworkPacket& packet, bool front)
{
    if (AuthStatus == NetworkAuth::Ok || !packet.CommandRequiresAuth())
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

void NetworkConnection::Disconnect() noexcept
{
    ShouldDisconnect = true;
}

bool NetworkConnection::IsValid() const
{
    return !ShouldDisconnect && Socket->GetStatus() == SocketStatus::Connected;
}

void NetworkConnection::SendQueuedData()
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

void NetworkConnection::ResetLastPacketTime() noexcept
{
    _lastPacketTime = Platform::GetTicks();
}

bool NetworkConnection::ReceivedPacketRecently() const noexcept
{
    #ifndef DEBUG
    constexpr auto kTimeoutMs = kNetworkNoDataTimeout * 1000;
    if (Platform::GetTicks() > _lastPacketTime + kTimeoutMs)
    {
        return false;
    }
    #endif
    return true;
}

const utf8* NetworkConnection::GetLastDisconnectReason() const noexcept
{
    return this->_lastDisconnectReason.c_str();
}

void NetworkConnection::SetLastDisconnectReason(std::string_view src)
{
    _lastDisconnectReason = src;
}

void NetworkConnection::SetLastDisconnectReason(const StringId string_id, void* args)
{
    char buffer[kNetworkDisconnectReasonBufSize];
    OpenRCT2::FormatStringLegacy(buffer, kNetworkDisconnectReasonBufSize, string_id, args);
    SetLastDisconnectReason(buffer);
}

void NetworkConnection::RecordPacketStats(const NetworkPacket& packet, bool sending)
{
    uint32_t packetSize = static_cast<uint32_t>(packet.BytesTransferred);
    NetworkStatisticsGroup trafficGroup;

    switch (packet.GetCommand())
    {
        case NetworkCommand::GameAction:
            trafficGroup = NetworkStatisticsGroup::Commands;
            break;
        case NetworkCommand::Map:
            trafficGroup = NetworkStatisticsGroup::MapData;
            break;
        default:
            trafficGroup = NetworkStatisticsGroup::Base;
            break;
    }

    if (sending)
    {
        Stats.bytesSent[EnumValue(trafficGroup)] += packetSize;
        Stats.bytesSent[EnumValue(NetworkStatisticsGroup::Total)] += packetSize;
    }
    else
    {
        Stats.bytesReceived[EnumValue(trafficGroup)] += packetSize;
        Stats.bytesReceived[EnumValue(NetworkStatisticsGroup::Total)] += packetSize;
    }
}

#endif

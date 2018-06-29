/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include "network.h"
#include "NetworkConnection.h"
#include "../core/String.hpp"

#include "../localisation/Localisation.h"
#include "../platform/platform.h"

constexpr size_t NETWORK_DISCONNECT_REASON_BUFFER_SIZE = 256;

NetworkConnection::NetworkConnection()
{
    ResetLastPacketTime();
}

NetworkConnection::~NetworkConnection()
{
    delete Socket;
    delete[] _lastDisconnectReason;
}

int32_t NetworkConnection::ReadPacket()
{
    if (InboundPacket.BytesTransferred < sizeof(InboundPacket.Size))
    {
        // read packet size
        void * buffer = &((char*)&InboundPacket.Size)[InboundPacket.BytesTransferred];
        size_t bufferLength = sizeof(InboundPacket.Size) - InboundPacket.BytesTransferred;
        size_t readBytes;
        NETWORK_READPACKET status = Socket->ReceiveData(buffer, bufferLength, &readBytes);
        if (status != NETWORK_READPACKET_SUCCESS)
        {
            return status;
        }

        InboundPacket.BytesTransferred += readBytes;
        if (InboundPacket.BytesTransferred == sizeof(InboundPacket.Size))
        {
            InboundPacket.Size = Convert::NetworkToHost(InboundPacket.Size);
            if (InboundPacket.Size == 0) // Can't have a size 0 packet
            {
                return NETWORK_READPACKET_DISCONNECTED;
            }
            InboundPacket.Data->resize(InboundPacket.Size);
        }
    }
    else
    {
        // read packet data
        if (InboundPacket.Data->capacity() > 0)
        {
            void * buffer = &InboundPacket.GetData()[InboundPacket.BytesTransferred - sizeof(InboundPacket.Size)];
            size_t bufferLength = sizeof(InboundPacket.Size) + InboundPacket.Size - InboundPacket.BytesTransferred;
            size_t readBytes;
            NETWORK_READPACKET status = Socket->ReceiveData(buffer, bufferLength, &readBytes);
            if (status != NETWORK_READPACKET_SUCCESS)
            {
                return status;
            }

            InboundPacket.BytesTransferred += readBytes;
        }
        if (InboundPacket.BytesTransferred == sizeof(InboundPacket.Size) + InboundPacket.Size)
        {
            _lastPacketTime = platform_get_ticks();
            return NETWORK_READPACKET_SUCCESS;
        }
    }
    return NETWORK_READPACKET_MORE_DATA;
}

bool NetworkConnection::SendPacket(NetworkPacket& packet)
{
    uint16_t sizen = Convert::HostToNetwork(packet.Size);
    std::vector<uint8_t> tosend;
    tosend.reserve(sizeof(sizen) + packet.Size);
    tosend.insert(tosend.end(), (uint8_t*)&sizen, (uint8_t*)&sizen + sizeof(sizen));
    tosend.insert(tosend.end(), packet.Data->begin(), packet.Data->end());

    const void * buffer = &tosend[packet.BytesTransferred];
    size_t bufferSize = tosend.size() - packet.BytesTransferred;
    size_t sent = Socket->SendData(buffer, bufferSize);
    if (sent > 0)
    {
        packet.BytesTransferred += sent;
    }
    return packet.BytesTransferred == tosend.size();
}

void NetworkConnection::QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front)
{
    if (AuthStatus == NETWORK_AUTH_OK || !packet->CommandRequiresAuth())
    {
        packet->Size = (uint16_t)packet->Data->size();
        if (front)
        {
            // If the first packet was already partially sent add new packet to second position
            if (!_outboundPackets.empty() && _outboundPackets.front()->BytesTransferred > 0)
            {
                auto it = _outboundPackets.begin();
                it++; // Second position
                _outboundPackets.insert(it, std::move(packet));
            }
            else
            {
                _outboundPackets.push_front(std::move(packet));
            }
        }
        else
        {
            _outboundPackets.push_back(std::move(packet));
        }
    }
}

void NetworkConnection::SendQueuedPackets()
{
    while (!_outboundPackets.empty() && SendPacket(*_outboundPackets.front()))
    {
        _outboundPackets.remove(_outboundPackets.front());
    }
}

void NetworkConnection::ResetLastPacketTime()
{
    _lastPacketTime = platform_get_ticks();
}

bool NetworkConnection::ReceivedPacketRecently()
{
#ifndef DEBUG
    if (platform_get_ticks() > _lastPacketTime + 7000)
    {
        return false;
    }
#endif
    return true;
}

const utf8 * NetworkConnection::GetLastDisconnectReason() const
{
    return this->_lastDisconnectReason;
}

void NetworkConnection::SetLastDisconnectReason(const utf8 * src)
{
    if (src == nullptr)
    {
        if (_lastDisconnectReason)
        {
            delete[] _lastDisconnectReason;
            _lastDisconnectReason = nullptr;
        }
        return;
    }

    if (_lastDisconnectReason == nullptr)
    {
        _lastDisconnectReason = new utf8[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
    }
    String::Set(_lastDisconnectReason, NETWORK_DISCONNECT_REASON_BUFFER_SIZE, src);
}

void NetworkConnection::SetLastDisconnectReason(const rct_string_id string_id, void *args)
{
    char buffer[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
    format_string(buffer, NETWORK_DISCONNECT_REASON_BUFFER_SIZE, string_id, args);
    SetLastDisconnectReason(buffer);
}

#endif

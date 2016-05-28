#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "NetworkConnection.h"
#include "../core/String.hpp"
#include <SDL.h>

extern "C"
{
    #include "../localisation/localisation.h"
}

constexpr size_t NETWORK_DISCONNECT_REASON_BUFFER_SIZE = 256;

NetworkConnection::NetworkConnection()
{
    authstatus = NETWORK_AUTH_NONE;
    player = 0;
    socket = INVALID_SOCKET;
    ResetLastPacketTime();
    last_disconnect_reason = nullptr;
}

NetworkConnection::~NetworkConnection()
{
    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
    }
    if (last_disconnect_reason)
    {
        delete[] last_disconnect_reason;
    }
}

int NetworkConnection::ReadPacket()
{
    if (inboundpacket.transferred < sizeof(inboundpacket.size))
    {
        // read packet size
        int readBytes = recv(socket, &((char*)&inboundpacket.size)[inboundpacket.transferred], sizeof(inboundpacket.size) - inboundpacket.transferred, 0);
        if (readBytes == SOCKET_ERROR || readBytes == 0)
        {
            if (LAST_SOCKET_ERROR() != EWOULDBLOCK && LAST_SOCKET_ERROR() != EAGAIN)
            {
                return NETWORK_READPACKET_DISCONNECTED;
            }
            else
            {
                return NETWORK_READPACKET_NO_DATA;
            }
        }
        inboundpacket.transferred += readBytes;
        if (inboundpacket.transferred == sizeof(inboundpacket.size))
        {
            inboundpacket.size = ntohs(inboundpacket.size);
            if (inboundpacket.size == 0) // Can't have a size 0 packet
            {
                return NETWORK_READPACKET_DISCONNECTED;
            }
            inboundpacket.data->resize(inboundpacket.size);
        }
    }
    else
    {
        // read packet data
        if (inboundpacket.data->capacity() > 0)
        {
            int readBytes = recv(socket,
                                 (char*)&inboundpacket.GetData()[inboundpacket.transferred - sizeof(inboundpacket.size)],
                                 sizeof(inboundpacket.size) + inboundpacket.size - inboundpacket.transferred,
                                 0);
            if (readBytes == SOCKET_ERROR || readBytes == 0)
            {
                if (LAST_SOCKET_ERROR() != EWOULDBLOCK && LAST_SOCKET_ERROR() != EAGAIN)
                {
                    return NETWORK_READPACKET_DISCONNECTED;
                }
                else
                {
                    return NETWORK_READPACKET_NO_DATA;
                }
            }
            inboundpacket.transferred += readBytes;
        }
        if (inboundpacket.transferred == sizeof(inboundpacket.size) + inboundpacket.size)
        {
            last_packet_time = SDL_GetTicks();
            return NETWORK_READPACKET_SUCCESS;
        }
    }
    return NETWORK_READPACKET_MORE_DATA;
}

bool NetworkConnection::SendPacket(NetworkPacket& packet)
{
    uint16 sizen = htons(packet.size);
    std::vector<uint8> tosend;
    tosend.reserve(sizeof(sizen) + packet.size);
    tosend.insert(tosend.end(), (uint8*)&sizen, (uint8*)&sizen + sizeof(sizen));
    tosend.insert(tosend.end(), packet.data->begin(), packet.data->end());
    while (true)
    {
        int sentBytes = send(socket, (const char*)&tosend[packet.transferred], tosend.size() - packet.transferred, FLAG_NO_PIPE);
        if (sentBytes == SOCKET_ERROR)
        {
            return false;
        }
        packet.transferred += sentBytes;
        if (packet.transferred == tosend.size())
        {
            return true;
        }
    }
    return false;
}

void NetworkConnection::QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front)
{
    if (authstatus == NETWORK_AUTH_OK || !packet->CommandRequiresAuth())
    {
        packet->size = (uint16)packet->data->size();
        if (front)
        {
            outboundpackets.push_front(std::move(packet));
        }
        else
        {
            outboundpackets.push_back(std::move(packet));
        }
    }
}

void NetworkConnection::SendQueuedPackets()
{
    while (outboundpackets.size() > 0 && SendPacket(*(outboundpackets.front()).get()))
    {
        outboundpackets.remove(outboundpackets.front());
    }
}

bool NetworkConnection::SetTCPNoDelay(bool on)
{
    return setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on)) == 0;
}

bool NetworkConnection::SetNonBlocking(bool on)
{
    return SetNonBlocking(socket, on);
}

bool NetworkConnection::SetNonBlocking(SOCKET socket, bool on)
{
#ifdef __WINDOWS__
    u_long nonblocking = on;
    return ioctlsocket(socket, FIONBIO, &nonblocking) == 0;
#else
    int flags = fcntl(socket, F_GETFL, 0);
    return fcntl(socket, F_SETFL, on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) == 0;
#endif
}

void NetworkConnection::ResetLastPacketTime()
{
    last_packet_time = SDL_GetTicks();
}

bool NetworkConnection::ReceivedPacketRecently()
{
#ifndef DEBUG
    if (SDL_TICKS_PASSED(SDL_GetTicks(), last_packet_time + 7000))
    {
        return false;
    }
#endif
    return true;
}

const utf8 * NetworkConnection::getLastDisconnectReason() const
{
    return this->last_disconnect_reason;
}

void NetworkConnection::setLastDisconnectReason(const utf8 * src)
{
    if (src == nullptr)
    {
        if (last_disconnect_reason)
        {
            delete[] last_disconnect_reason;
            last_disconnect_reason = nullptr;
        }
        return;
    }

    if (last_disconnect_reason == nullptr)
    {
        last_disconnect_reason = new utf8[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
    }
    String::Set(last_disconnect_reason, NETWORK_DISCONNECT_REASON_BUFFER_SIZE, src);
}

void NetworkConnection::setLastDisconnectReason(const rct_string_id string_id, void *args)
{
    char buffer[NETWORK_DISCONNECT_REASON_BUFFER_SIZE];
    format_string(buffer, string_id, args);
    setLastDisconnectReason(buffer);
}

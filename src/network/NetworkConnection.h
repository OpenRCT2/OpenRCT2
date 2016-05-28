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

#pragma once

#include <list>
#include <memory>
#include <vector>
#include "NetworkTypes.h"
#include "NetworkKey.h"
#include "NetworkPacket.h"
#include "../common.h"

class NetworkPlayer;

class NetworkConnection
{
public:
    SOCKET              socket = INVALID_SOCKET;
    NetworkPacket       inboundpacket;
    NETWORK_AUTH        authstatus = NETWORK_AUTH_NONE;
    NetworkPlayer *     player;
    uint32              ping_time = 0;
    NetworkKey          key;
    std::vector<uint8>  challenge;

    NetworkConnection();
    ~NetworkConnection();

    int  ReadPacket();
    void QueuePacket(std::unique_ptr<NetworkPacket> packet, bool front = false);
    void SendQueuedPackets();
    bool SetTCPNoDelay(bool on);
    bool SetNonBlocking(bool on);
    static bool SetNonBlocking(SOCKET socket, bool on);
    void ResetLastPacketTime();
    bool ReceivedPacketRecently();

    const utf8 * getLastDisconnectReason() const;
    void setLastDisconnectReason(const utf8 * src);
    void setLastDisconnectReason(const rct_string_id string_id, void * args = nullptr);

private:
    utf8 * last_disconnect_reason;

    bool SendPacket(NetworkPacket &packet);
    std::list<std::unique_ptr<NetworkPacket>> outboundpackets;
    uint32 last_packet_time;
};

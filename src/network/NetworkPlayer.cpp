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

#ifndef DISABLE_NETWORK

#include "NetworkPacket.h"
#include "NetworkPlayer.h"

extern "C"
{
    #include "../interface/window.h"
    #include "../localisation/localisation.h"
}

void NetworkPlayer::SetName(const std::string &name)
{
    // 36 == 31 + strlen(" #255");
    NetworkPlayer::name = name.substr(0, 36);
    utf8_remove_format_codes((utf8 *)NetworkPlayer::name.data(), false);
}

void NetworkPlayer::Read(NetworkPacket &packet)
{
    const utf8 * name = packet.ReadString();
    SetName(name);
    packet >> id >> flags >> group;
}

void NetworkPlayer::Write(NetworkPacket &packet)
{
    packet.WriteString((const char*)name.c_str());
    packet << id << flags << group;
}

void NetworkPlayer::AddMoneySpent(money32 cost)
{
    money_spent += cost;
    commands_ran++;
    window_invalidate_by_number(WC_PLAYER, id);
}

#endif
